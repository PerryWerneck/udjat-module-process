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
 #include <udjat/tools/system/stat.h>

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

		try {

			//
			// Get total CPU usage.
			//
			float sysusage = 0;

			System::Stat stat;

			if(system.running || system.idle) {

				float running = (float) (stat.getRunning() - system.running);
				float idle = (float) (stat.getIdle() - system.idle);
				sysusage = this->system.cpu = (running / (running+idle));

			}

			system.running = stat.getRunning();
			system.idle = stat.getIdle();

#ifdef DEBUG
			cout << "Total CPU usage: " << (sysusage*100) << "%" << endl;
#endif // DEBUG

			if (update.cpu_use_per_process) {

				//
				// Get CPU usage by pid.
				//

				struct Entry {

					Identifier &info;
					float usage;

					Entry(Identifier &i, float u) : info(i), usage(u) {
					}

				};

				list<Entry> pids;

				unsigned long long totaltime = 0;

				for(auto ix = identifiers.begin(); ix != identifiers.end(); ix++) {

					Identifier::Stat stats(ix->refresh());

					unsigned long long cpu = (stats.utime + stats.stime);

					if(ix->last.cpu && cpu >= ix->last.cpu) {
						unsigned long long time = (cpu - ix->last.cpu);
						totaltime += time;
						float cpu = (float) time;
						pids.emplace_back(*ix,cpu);
					} else {
						ix->usage.cpu = 0;
					}
					ix->last.cpu = cpu;	// Store value for next check.

				}

#ifdef DEBUG
				cout << "Total time=" << totaltime << " pids=" << pids.size() << endl;
#endif // DEBUG

				if(sysusage && totaltime) {

#ifdef DEBUG
					cout << "Updating usage by pid" << endl;
#endif // DEBUG

					// sysusage ........... totaltime
					// info.usage.cpu ..... ix->usage

					for(auto ix = pids.begin(); ix != pids.end(); ix++) {
						ix->info.usage.cpu = (ix->usage * sysusage) / ((float) totaltime);
					}

				}
			}

		} catch(const exception &e) {

			cerr << "Error '" << e.what() << "' while updating process info" << endl;

		}


	}


 }

