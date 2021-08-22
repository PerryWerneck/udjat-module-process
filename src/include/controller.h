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
 #include <udjat/process/identifier.h>
 #include <mutex>
 #include <list>

 namespace Udjat {

	namespace Process {

		class Controller {
		private:
			static std::recursive_mutex guard;

			Controller();

			struct {
				/// @brief Update process CPU use.
				bool cpu_use_per_process = true;
			} update;

			/// @brief Get pid list.
			static void load(std::list<pid_t> &pids);

			/// @brief Update process list.
			void reload() noexcept;

			/// @brief Netlink socket
			int sock = -1;

			/// @brief Process identifiers.
			std::list<Identifier> identifiers;

			/// @brief Active agents.
			std::list<Agent *> agents;

			/// @brief Update CPU usage.
			void refresh() noexcept;

			void onInsert(Identifier &identifier);

			void insert(const pid_t pid) noexcept;
			void remove(const pid_t pid) noexcept;

			/// @brief System stats on last update.
			struct {
				float cpu = 0;				///< @brief System CPU usage.
				unsigned long running = 0;
				unsigned long idle = 0;
			} system;

		public:

			static Controller & getInstance();
			~Controller();

			void insert(Agent *agent);
			void remove(Agent *agent);

			size_t count(const Process::Identifier::State state);

			inline float getSystemCpuUse() const noexcept {
				return system.cpu;
			}

			Identifier * find(const pid_t pid);

		};

	}


 }


