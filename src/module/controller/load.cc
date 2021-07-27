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

 #include <config.h>
 #include <controller.h>
 #include <dirent.h>
 #include <sys/types.h> // for opendir(), readdir(), closedir()
 #include <iostream>

 using namespace std;
 namespace Udjat {

 	bool isNumeric(const char *name) {

		while(*name) {

			if(*name < '0' || *name > '9')
				return false;

			name++;
		}

		return true;
 	}

	void Process::Agent::Controller::load(std::list<pid_t> &entries) {

		entries.clear(); // Just in case

		// https://stackoverflow.com/questions/939778/linux-api-to-list-running-processes
		DIR *dir = opendir("/proc");
		if(!dir) {
			throw std::system_error(errno, std::system_category(), "Can't open /proc");
		}

		struct dirent *entity;
		while( (entity = readdir(dir))) {
			if( !(entity->d_type == DT_DIR && isNumeric(entity->d_name)) )
				continue;
			entries.emplace_back( (pid_t) atoi(entity->d_name));
		}

		closedir(dir);

#ifdef DEBUG
		cout << "Loaded " << entries.size() << " entries from /proc" << endl;
#endif // DEBUG

	}

	static bool search(const std::list<pid_t> &entries, const Process::Identifier &pid) {

		for(auto e = entries.begin(); e != entries.end(); e++) {
			if(*e == pid)
				return true;
		}

		return false;
	}

	static bool search(const std::list<Process::Identifier> &entries, const pid_t pid) {

		for(auto e = entries.begin(); e != entries.end(); e++) {
			if(*e == pid)
				return true;
		}

		return false;
	}

	void Process::Agent::Controller::reload() noexcept {

		try {

			// get updated list.
			std::list<pid_t> current;
			load(current);

			// Compare current list with the internal one.
			{
				lock_guard<recursive_mutex> lock(guard);

				// Remove finished processes.
				this->entries.remove_if([current](Identifier &entry){
					return !search(current,entry);
				});

				// Remove already registered entries.
				current.remove_if([this](pid_t &entry){
					return search(this->entries,entry);
				});

				if(!current.empty()) {

					// Add new processes to active list.
					//this->entries.splice(this->entries.end(),current,current.begin(),current.end());
					for(auto ix = current.begin(); ix != current.end(); ix++) {
						insert(*ix);
					}

				}

			}

		} catch(const exception &e) {

			cerr << "Error '" << e.what() << "' loading process list" << endl;
			entries.clear();
		}

	}

 }

