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

 #include "private.h"
 #include <controller.h>

 namespace Udjat {

	Process::Agent::Agent() {
	}

	Process::Agent::Agent(const pugi::xml_node &node) : Agent() {
		load(node);
	}

	Process::Agent::~Agent() {
		Process::Controller::getInstance().remove(this);
	}

	bool Process::Agent::probe(const Identifier &ident) const noexcept {

		string exe;

		try {

			exe = ident.exename();

		} catch(...) {

			return false;

		}

		return probe(exe.c_str());
	}

	Process::Identifier::State Process::Agent::getState() const noexcept {

		if(pid) {
			return pid->getState();
		}

		return Process::Identifier::Dead;

	}

	void Process::Agent::start() {
#ifdef DEBUG
		info("{}","starting");
#endif // DEBUG
		Process::Controller::getInstance().insert(this);
	}

	void Process::Agent::get(const Request &request, Response &response) {

		super::get(request,response);

		if(pid) {
			Identifier::Stat(pid).get(response);
		} else {
			Identifier::Stat().get(response);
		}

		//response["vsize"] = stat.vsize;
		//response["mode"] = Identifier::getStateName(getState()).name;

	}

	/*
	bool Process::Agent::probe(const Identifier &ident) noexcept {
		return false;
	}
	*/

	void Process::Agent::setIdentifier(Identifier *pid) {

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

	std::shared_ptr<Abstract::State> Process::Agent::stateFromValue() const {

		for(auto state : this->states) {
			if(state->test(*this))
				return state;
		}

		return Abstract::Agent::stateFromValue();

	}

	bool Process::Agent::hasStates() const noexcept {
		return !states.empty();
	}

 }
