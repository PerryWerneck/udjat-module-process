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

	recursive_mutex Process::Identifier::guard;

	Process::Identifier::~Identifier() {
		set(STATE_DEAD);
	}

	std::string Process::Identifier::exename() const {

		string pathname{"/proc/"};
		pathname += std::to_string((unsigned int) pid) + "/exe";

		char name[4096];

		ssize_t sz = readlink(pathname.c_str(), name, 4095);
		if(sz > 0) {
			name[sz] = 0;
		} else {
			cerr << "Error '" << strerror(errno) << "' getting exename for pid " << pid << endl;
			return string{"pid"} + std::to_string((unsigned int) pid);
		}

		return name;

	}

	void Process::Identifier::reset() {

		set(STATE_UNDEFINED);

		{
			lock_guard<recursive_mutex> lock(guard);
			for(auto listener : listeners) {
				listener->setCPU(0);
			}
		}

	}

	/*
	unsigned long Process::Identifier::refresh() {

		try {

			// Read /proc/pid/stat.
			Stat stat(pid);

			// Set process state.
			set((State) stat.state);

			// Update CPU time.
			unsigned long current = (stat.utime + stat.stime);

			if(cpu.last && current > cpu.last) {
				cpu.current = (current - cpu.last);
			} else {
				cpu.current = 0;
			}

			if(!cpu.current) {
				lock_guard<recursive_mutex> lock(guard);
				for(auto listener : listeners) {
					listener->setCPU(0);
				}
			}

			cpu.last = current;

		} catch(...) {

			reset();
			throw;

		}

		return cpu.current;
	}
	*/

	void Process::Identifier::setCpu(float percent) {
	}

	void Process::Identifier::set(const State state) {

		if(state == this->state)
			return;

		this->state = state;

		// State has changed.
		{
			lock_guard<recursive_mutex> lock(guard);
			for(auto listener : listeners) {
				listener->set(state);
			}
		}

	}

 }

