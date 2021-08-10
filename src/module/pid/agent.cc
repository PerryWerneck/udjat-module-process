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
 #include <udjat/process/agent.h>
 #include <controller.h>

 using namespace std;

 namespace Udjat {

	Process::Agent::Agent() {
		Process::Controller::getInstance().insert(this);
	}

	Process::Agent::~Agent() {
		Process::Controller::getInstance().remove(this);
	}

	void Process::Agent::get(const Request &request, Response &response) {

		Identifier::Stat stat(pid);

		response["vsize"] = stat.vsize;


	}

	bool Process::Agent::probe(const Identifier &ident) noexcept {
		return false;
	}

	void Process::Agent::setIdentifier(const Identifier *pid) {

		if(pid == this->pid) {
			return;
		}

		this->pid = pid;

		// Notify state change.
		if(pid) {
			info("Detected on pid '{}'", ((pid_t) *pid));
		} else {
			info("{}","Not available");
		}

		// Mark as updated and changed.
		updated(true);
	}


 }
