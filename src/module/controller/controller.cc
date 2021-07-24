/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2021 Perry Werneck <perry.werneck@gmail.com>
 * Derived from exec-notify.c by Matt Helsley
 * Original copyright notice follows:
 *
 * Copyright (C) Matt Helsley, IBM Corp. 2005
 * Derived from fcctl.c by Guillaume Thouvenin
 * Original copyright notice follows:
 *
 * Copyright (C) 2005 BULL SA.
 * Written by Guillaume Thouvenin <guillaume.thouvenin@bull.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * References:
 *
 * <https://gist.github.com/L-P/9487407>
 * <http://netsplit.com/the-proc-connector-and-socket-filters>
 *
 */

 #include <config.h>
 #include <controller.h>
 #include <iostream>
 #include <unistd.h>
 #include <udjat/tools/mainloop.h>

 #include <sys/socket.h>
 #include <sys/types.h>

 #include <linux/connector.h>
 #include <linux/netlink.h>
 #include <linux/cn_proc.h>

 #define SEND_MESSAGE_LEN (NLMSG_LENGTH(sizeof(struct cn_msg) + \
				       sizeof(enum proc_cn_mcast_op)))
 #define RECV_MESSAGE_LEN (NLMSG_LENGTH(sizeof(struct cn_msg) + \
				       sizeof(struct proc_event)))

 #define SEND_MESSAGE_SIZE    (NLMSG_SPACE(SEND_MESSAGE_LEN))
 #define RECV_MESSAGE_SIZE    (NLMSG_SPACE(RECV_MESSAGE_LEN))

 #define max(x,y) ((y)<(x)?(x):(y))
 #define min(x,y) ((y)>(x)?(x):(y))

 #define BUFF_SIZE (max(max(SEND_MESSAGE_SIZE, RECV_MESSAGE_SIZE), 1024))
 #define MIN_RECV_SIZE (min(SEND_MESSAGE_SIZE, RECV_MESSAGE_SIZE))

 #define PROC_CN_MCAST_LISTEN (1)
 #define PROC_CN_MCAST_IGNORE (2)

 using namespace std;

 namespace Udjat {

	std::mutex Process::Agent::Controller::guard;

	Process::Agent::Controller::Controller() {
		cout << "Process controller is starting" << endl;

		// Watch kernel process list.

		// Create an endpoint for communication. Use the kernel user
		// interface device (PF_NETLINK) which is a datagram oriented
		// service (SOCK_DGRAM). The protocol used is the connector
		// protocol (NETLINK_CONNECTOR)
		sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
		if(sock < 0) {

			clog << "Error '" << strerror (errno) << "' creating netlink socket" << endl;

		} else {

			// http://man7.org/linux/man-pages/man7/netlink.7.html
			// https://github.com/reubenhwk/radvd/blob/master/netlink.c
			static const int val = 1;
			if (setsockopt(sock, SOL_NETLINK, NETLINK_NO_ENOBUFS, &val, sizeof(val)) < 0) {
				clog << "Unable to setsockopt NETLINK_NO_ENOBUFS: " << strerror(errno) << endl;
			}

			try {

				struct sockaddr_nl my_nla;
				memset(&my_nla,0,sizeof(my_nla));

				// Faz o bind
				my_nla.nl_family = AF_NETLINK;
				my_nla.nl_groups = CN_IDX_PROC;
				my_nla.nl_pid = getpid();

				if(bind(sock, (struct sockaddr *)&my_nla, sizeof(my_nla)) < 0) {
					throw std::system_error(errno, std::system_category(), "Can't bind process list connector");
				}

				char buff[BUFF_SIZE];
				struct nlmsghdr			* nl_hdr	= (struct nlmsghdr *) buff;
				struct cn_msg			* cn_hdr	= (struct cn_msg *)NLMSG_DATA(nl_hdr);
				enum proc_cn_mcast_op	* mcop_msg	= (enum proc_cn_mcast_op*)&cn_hdr->data[0];

#ifdef DEBUG
				cout << "sending proc connector: PROC_CN_MCAST_LISTEN" << endl;
#endif // DEBUG

				memset(buff, 0, sizeof(buff));
				*mcop_msg = (proc_cn_mcast_op) PROC_CN_MCAST_LISTEN;

				// fill the netlink header
				nl_hdr->nlmsg_len = SEND_MESSAGE_LEN;
				nl_hdr->nlmsg_type = NLMSG_DONE;
				nl_hdr->nlmsg_flags = 0;
				nl_hdr->nlmsg_seq = 0;
				nl_hdr->nlmsg_pid = getpid();

				// fill the connector header
				cn_hdr->id.idx = CN_IDX_PROC;
				cn_hdr->id.val = CN_VAL_PROC;
				cn_hdr->seq = 0;
				cn_hdr->ack = 0;
				cn_hdr->len = sizeof(enum proc_cn_mcast_op);

				if(send(sock, nl_hdr, nl_hdr->nlmsg_len, 0) != nl_hdr->nlmsg_len) {
					throw std::system_error(errno, std::system_category(), "Failed to send proc connector mcast ctl op");
				}

				MainLoop::getInstance().insert(this,sock,MainLoop::oninput,[this](const MainLoop::Event &event) {
					lock_guard<mutex> lock(guard);

					//
					// Process kernel event
					//
					struct sockaddr_nl from_nla;
					socklen_t from_nla_len;
					char buff[BUFF_SIZE];

					memset(buff,0,sizeof(buff));
					memset(&from_nla,0,sizeof(from_nla));

					from_nla.nl_family	= AF_NETLINK;
					from_nla.nl_groups	= CN_IDX_PROC;
					from_nla.nl_pid 	= 1;
					from_nla_len		= sizeof(from_nla);

					ssize_t recv_len = recvfrom(sock, buff, BUFF_SIZE, MSG_DONTWAIT,(struct sockaddr*)&from_nla, &from_nla_len);

					if(recv_len < 0) {
						cerr << "Error '" << strerror(errno) << "' reading proc connector event" << endl;
						return true;
					}

					if(recv_len < 1 || from_nla.nl_pid != 0)
						return true;

					// Read messages.
					struct nlmsghdr		* nlh = (struct nlmsghdr*) buff;
					struct proc_event	* ev;
					struct cn_msg		* cn_hdr;

					while (NLMSG_OK(nlh, recv_len)) {

						cn_hdr = (struct cn_msg	*) NLMSG_DATA(nlh);

						if (nlh->nlmsg_type == NLMSG_NOOP)
							continue;

						if ((nlh->nlmsg_type == NLMSG_ERROR) || (nlh->nlmsg_type == NLMSG_OVERRUN))
							break;

						//
						// Processa a mensagem recebida.
						//
						ev = (struct proc_event *) cn_hdr->data;

						#pragma GCC diagnostic push
						#pragma GCC diagnostic ignored "-Wswitch"
						switch(ev->what) {
						case proc_event::PROC_EVENT_FORK:
							break;

						case proc_event::PROC_EVENT_EXEC:
#ifdef DEBUG
							cout << "Process '" << ((pid_t) ev->event_data.exec.process_pid) << "' starts" << endl;
#endif // DEBUG
							//insert((pid_t) ev->event_data.exec.process_pid, true);
							break;

						case proc_event::PROC_EVENT_EXIT:
#ifdef DEBUG
							cout << "Process '" << ((pid_t) ev->event_data.exec.process_pid) << "' ends" << endl;
#endif // DEBUG
							// remove((pid_t) ev->event_data.exec.process_pid);
							break;

			/*
						case proc_event::PROC_EVENT_UID:
						case proc_event::PROC_EVENT_NONE:
						case proc_event::PROC_EVENT_GID:
						case proc_event::PROC_EVENT_SID:
							break;
			*/

			/*

#ifdef PROC_EVENT_COMM
						case proc_event::PROC_EVENT_COMM:
							break;
#endif // PROC_EVENT_COMM
			*/

#ifdef HAVE_PROC_EVENT_PTRACE
						// http://lists.openwall.net/netdev/2011/07/12/105
						case proc_event::PROC_EVENT_PTRACE:
//							trace(ev->event_data.id.process_pid,ev->event_data.ptrace.tracer_pid);
							break;
#endif // HAVE_PROC_EVENT_PTRACE

#ifdef HAVE_PROC_EVENT_COREDUMP
						case proc_event::PROC_EVENT_COREDUMP:
//							warning("Coredump detected on PID %u", (unsigned int) ev->event_data.id.process_pid);
//							fireEvent(onCoreDump, (pid_t) ev->event_data.id.process_pid);
							break;
#endif // HAVE_PROC_EVENT_COREDUMP
						}
						#pragma GCC diagnostic pop

						// More?
						if (nlh->nlmsg_type == NLMSG_DONE)
							break;

						nlh = NLMSG_NEXT(nlh, recv_len);
					}

					return true;
				});

			} catch(const exception &e) {

				clog << e.what() << endl;
				::close(sock);
				sock = -1;

			}

		}

		// Starting data colecting timer.
		MainLoop::getInstance().insert(this,1000,[this](){
			try {

				onTimer();

			} catch(const exception &e) {

				cerr << "Error '" << e.what() << "' updating process list" << endl;

			} catch(...) {

				cerr << "Unexpected errror updating process list" << endl;

			}

			return true;
		});
	}

	Process::Agent::Controller::~Controller() {
		cout << "Process controller is stopping" << endl;
		MainLoop::getInstance().remove(this);


		if(sock > 0) {
			::close(sock);
		}
	}

	Process::Agent::Controller & Process::Agent::Controller::getInstance() {
		lock_guard<mutex> lock(guard);
		static Controller instance;
		return instance;
	}

	void Process::Agent::Controller::insert(Process::Agent *agent) {
		lock_guard<mutex> lock(guard);
		agents.push_back(agent);
	}

	void Process::Agent::Controller::remove(Process::Agent *agent) {
		lock_guard<mutex> lock(guard);
		agents.remove_if([agent](Agent *a) {
			return a == agent;
		});
	}

	void Process::Agent::Controller::onTimer() {
		lock_guard<mutex> lock(guard);


	}

 }

