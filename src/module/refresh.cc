/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2021 Perry Werneck <perry.werneck@gmail.com>
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

 #include <controller.h>
 #include <unistd.h>
 #include <string>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>

 using namespace std;

 namespace Udjat {

	Process::Identifier::Stat Process::Identifier::refresh() {

		try {

			// Read /proc/pid/stat.
			Stat pidstat(pid);

			// Set process state.
			set((State) pidstat.state);

			return pidstat;

		} catch(...) {

			reset();
			throw;

		}

	}

	void Process::Agent::Controller::refresh() noexcept {

		lock_guard<recursive_mutex> lock(guard);

		struct Entry {

			Identifier &info;
			Identifier::Stat stats;
			unsigned long long time;

			Entry(Identifier &i) : info(i), stats(i.refresh()) {
			}

		};

		list<Entry> stats;

		try {

			//
			// Get total CPU usage.
			//
			Process::Stat stat;

			//
			// Get CPU usage by pid.
			//
			unsigned long long totaltime = 0;

			for(auto ix = entries.begin(); ix != entries.end(); ix++) {
				auto entry = stats.emplace_back(*ix);

				unsigned long long t = (entry.stats.utime + entry.stats.stime);

				if(entry.info.last.time && t > entry.info.last.time) {
					entry.time = t - entry.info.last.time;
					totaltime += entry.time;
				}
				entry.info.last.time = t;
			}

			cout << "Total time=" << totaltime << endl;


		} catch(const exception &e) {

			cerr << "Error '" << e.what() << "' while updating process info" << endl;

		}


	}


 }

