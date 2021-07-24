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

 #pragma once

 #include <udjat/defs.h>
 #include <udjat/process/agent.h>
 #include <mutex>
 #include <list>

 namespace Udjat {

	class Process::Agent::Controller {
	private:
		static std::recursive_mutex guard;

		Controller();

		/// @brief A single process on the list
		class Entry {
		private:
			pid_t pid;

		public:
			constexpr Entry(pid_t p) : pid(p) {
			}

			constexpr bool operator==(const pid_t pid) const {
				return this->pid == pid;
			}


		};

		/// @brief Current processes.
		std::list<Entry> entries;

		/// @brief Get process.

		/// @brief Netlink socket
		int sock = -1;

		/// @brief List of process agents.
		std::list<Agent *> agents;

		/// @brief timer call.
		void onTimer();

		void insert(const pid_t pid) noexcept;
		void remove(const pid_t pid) noexcept;

	public:

		static Controller & getInstance();
		~Controller();

		void insert(Agent *agent);
		void remove(Agent *agent);

	};

 }


