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

	std::recursive_mutex Process::Controller::guard;

	Process::Controller & Process::Controller::getInstance() {
		lock_guard<recursive_mutex> lock(guard);
		static Controller instance;
		return instance;
	}

	void Process::Controller::insert(Process::Agent *agent) {
		lock_guard<recursive_mutex> lock(guard);
		agents.push_back(agent);

		for(auto identifier = identifiers.begin(); identifier != identifiers.end(); identifier++) {
			if(agent->probe(*identifier)) {
				agent->setIdentifier(&(*identifier));
				break;
			}
		}

	}

	void Process::Controller::remove(Process::Agent *agent) {
		lock_guard<recursive_mutex> lock(guard);
		agents.remove_if([agent](Agent *a) {
			return a == agent;
		});
	}

	void Process::Controller::Controller::insert(pid_t pid) noexcept {

		lock_guard<recursive_mutex> lock(guard);

		try {

			auto element = identifiers.emplace_back(pid);

#ifdef DEBUG
			cout << "Adding process " << pid << " - " << element.exename() << endl;
#endif // DEBUG

			for(auto agent : agents) {

				if(!agent->pid && agent->probe(element)) {
					agent->setIdentifier(&element);
				}

			}

		} catch(const exception &e) {

			cerr << "Error '" << e.what() << "' inserting pid " << pid << endl;

		}


	}

	void Process::Controller::Controller::remove(pid_t pid) noexcept {

		try {

			lock_guard<recursive_mutex> lock(guard);

			identifiers.remove_if([this,pid](Identifier &e) {

				if(e == pid) {
					for(auto agent : agents) {
						if(agent->pid == &e) {
							agent->setIdentifier(nullptr);
						}
					}
					return true;
				}
				return false;

			});

		} catch(const exception &e) {

			cerr << "Error '" << e.what() << "' inserting pid " << pid << endl;

		}

	}


 }

