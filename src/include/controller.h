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

	namespace Process {

		class Agent::Controller {
		private:
			static std::recursive_mutex guard;

			Controller();

			/// @brief Get pid list.
			static void load(std::list<pid_t> &pids);

			/// @brief Update process list.
			void reload() noexcept;

			/// @brief Current processes.
			std::list<Identifier> entries;

			/// @brief Get process.

			/// @brief Netlink socket
			int sock = -1;

			/// @brief List of process agents.
			std::list<Agent *> agents;

			/// @brief Update CPU usage.
			void refresh() noexcept;

			void insert(const pid_t pid) noexcept;
			void remove(const pid_t pid) noexcept;

		public:

			static Controller & getInstance();
			~Controller();

			void insert(Agent *agent);
			void remove(Agent *agent);

		};

	}


 }


