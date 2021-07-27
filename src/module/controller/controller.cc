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

 using namespace std;

 namespace Udjat {

	std::recursive_mutex Process::Agent::Controller::guard;

	Process::Agent::Controller & Process::Agent::Controller::getInstance() {
		lock_guard<recursive_mutex> lock(guard);
		static Controller instance;
		return instance;
	}

	void Process::Agent::Controller::insert(Process::Agent *agent) {
		lock_guard<recursive_mutex> lock(guard);
		agents.push_back(agent);
	}

	void Process::Agent::Controller::remove(Process::Agent *agent) {
		lock_guard<recursive_mutex> lock(guard);
		agents.remove_if([agent](Agent *a) {
			return a == agent;
		});
	}

	void Process::Agent::Controller::Controller::insert(pid_t pid) noexcept {

		lock_guard<recursive_mutex> lock(guard);

		try {

			auto element = entries.emplace_back(pid);
			element.refresh();

#ifdef DEBUG
			cout << "Adding process " << pid << " - " << element.exename() << endl;
#endif // DEBUG

		} catch(const exception &e) {

			cerr << "Error '" << e.what() << "' inserting pid " << pid << endl;

		}

	}

	void Process::Agent::Controller::Controller::remove(pid_t pid) noexcept {

		try {

			lock_guard<recursive_mutex> lock(guard);

			entries.remove_if([pid](Identifier &e) {
				return e == pid;
			});


		} catch(const exception &e) {

			cerr << "Error '" << e.what() << "' inserting pid " << pid << endl;

		}

	}


 }

