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

	Process::StateCounterAgent::StateCounterAgent(const char *statename, const pugi::xml_node &node) : Udjat::Agent<unsigned int>(node), state(Process::Identifier::StateFactory(statename)) {
	}

	void Process::StateCounterAgent::start() {
		super::start(Process::Controller::getInstance().count(state));
	}

	bool Process::StateCounterAgent::refresh() {
		return set(Process::Controller::getInstance().count(state));
	}

 }

