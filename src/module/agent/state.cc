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
#ifdef DEBUG
			agent.info() << "State=" << agent.getState() << " mine=" << state << endl;
#endif // DEBUG
			return agent.getState() == state;
		}

	};

	/// @brief Agent state based on process availability.
	class ProcessAvailable : public Process::Agent::State {
	private:
		bool required;

	public:
		ProcessAvailable(bool r, const pugi::xml_node &node) : Process::Agent::State(node), required(r) {
		}

		bool test(const Process::Agent &agent) const noexcept override {
			auto state = agent.getState();
			bool available = (state != Pid::Dead && state != Pid::DeadCompat && state != Pid::Zombie && state != Pid::Stopped);
#ifdef DEBUG
			agent.info() << "State=" << agent.getState() << " available=" << (available ? "yes" : "no") << endl;
#endif // DEBUG
			return available == required;
		}

	};

	/// @brief State by process usage in %
	class ProcessUsage : public Process::Agent::State {
	protected:
		float from, to;

		bool compare(float value) const noexcept {
			return value >= from && value <= to;
		}

	public:
		ProcessUsage(const pugi::xml_node &node) : Process::Agent::State(node) {
			parse_range(node,from,to);
		}

	};

	void Process::Agent::append_state(const pugi::xml_node &node) {

		pugi::xml_attribute attribute;


		// Agent state by process
		attribute = node.attribute("process-state");
		if(attribute) {

			const char *attr = attribute.as_string();

			if(strcasecmp(attr,"available") == 0) {
				states.push_back(make_shared<ProcessAvailable>(true,node));
			} else if(strcasecmp(attr,"not-available") == 0) {
				states.push_back(make_shared<ProcessAvailable>(false,node));
			} else {
				states.push_back(make_shared<ProcessState>(attribute.as_string(),node));
			}
			return;
		}

		attribute = node.attribute("field-name");
		if(attribute) {
			Process::Agent::Field field = Process::Agent::getField(attribute.as_string(Process::Agent::fieldNames[0]));

			/// @brief State based on field value.
			class Value : public Process::Agent::State {
			private:
				Process::Agent::Field field;
				unsigned long long from = 0;
				unsigned long long to = 0;

			public:
				Value(Process::Agent::Field f, const pugi::xml_node &node) : Process::Agent::State(node), field(f) {
					parse_byte_range(node,from,to);
				}

				bool test(const Process::Agent &agent) const noexcept override {
					unsigned long long value = agent.getValue(field);
					return value >= from && value <= to;
				}

			};

			states.push_back(make_shared<Value>(field, node));

		}

	}

	std::shared_ptr<Abstract::State> Process::Agent::stateFromValue() const {

		for(auto state : this->states) {
			if(state->test(*this))
				return state;
		}

#ifdef DEBUG
		info() << "Using default state" << endl;
#endif // DEBUG
		return Abstract::Agent::stateFromValue();

	}

 }
