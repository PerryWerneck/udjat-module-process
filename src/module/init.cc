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
 #include <udjat/agent.h>
 #include <udjat/process/agent.h>
 #include <udjat/module.h>
 #include <udjat/factory.h>
 #include <udjat/moduleinfo.h>

 using namespace std;

 static const Udjat::ModuleInfo moduleinfo{"Process information agents"};

 /// @brief Register udjat module.
 Udjat::Module * udjat_module_init() {

	class Module : public Udjat::Module, public Udjat::Factory {
	private:

	public:
		Module() : Udjat::Module("process",moduleinfo), Factory("process",moduleinfo) {
		};

		virtual ~Module() {
		}

		std::shared_ptr<Udjat::Abstract::Agent> AgentFactory(const Udjat::Abstract::Object UDJAT_UNUSED(&parent), const pugi::xml_node &node) const override {
			return Udjat::Process::Agent::AgentFactory(node);
		}


	};

	return new Module();
 }


