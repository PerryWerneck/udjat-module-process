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

 using namespace std;

 namespace Udjat {

	std::mutex Process::Agent::Controller::guard;

	Process::Agent::Controller::Controller() {
	}

	Process::Agent::Controller & Process::Agent::Controller::getInstance() {
		lock_guard<mutex> lock(guard);
		static Controller instance;
		return instance;
	}

	Process::Agent::Controller::~Controller() {

	}

	void Process::Agent::Controller::insert(Process::Agent *agent) {
		lock_guard<mutex> lock(guard);
		agents.push_back(agent);
	}

	void Process::Agent::Controller::remove(Process::Agent *agent) {
		lock_guard<mutex> lock(guard);
		agents.remove_if([agent](Agent *a) {
			return a == agent;
		});
	}

 }

