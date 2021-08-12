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

 using Pid = Udjat::Process::Identifier;

 namespace Udjat {

	/// @brief Agent state based on process state.
	class ProcessState : public Process::Agent::State {
	private:
		Process::Identifier state = Pid::Undefined;

	public:
		ProcessState(const char *s, const pugi::xml_node &node) : Process::Agent::State(node), state(Process::Identifier::getState(s)) {
		}

		bool test(const Process::Agent &agent) const noexcept override {
			return agent.getState() == state;
		}

	};

	/// @brief Agent state based on process availability.
	class ProcessAvailable : public Process::Agent::State {
	private:

	public:
		ProcessAvailable(const pugi::xml_node &node) : Process::Agent::State(node) {
		}

		bool test(const Process::Agent &agent) const noexcept override {
			auto state = agent.getState();
			return state != Pid::Dead && state != Pid::DeadCompat && state != Pid::Zombie && state != Pid::Stopped;
		}

	};

	void Process::Agent::append_state(const pugi::xml_node &node) {

		pugi::xml_attribute attribute;


		// Agent state by process
		attribute = node.attribute("process-state");
		if(attribute) {

			const char *attr = attribute.as_string();

			if(strcasecmp(attr,"available")) {
				states.push_back(make_shared<ProcessState>(attribute.as_string(),node));
			} else {
				states.push_back(make_shared<ProcessAvailable>(node));
			}
			return;
		}

		// Agent state by CPU Use


		// Agent state by Memory Use


		// Agent state by swap use.

	}


 }
