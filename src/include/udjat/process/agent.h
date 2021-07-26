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
 #include <udjat/factory.h>
 #include <udjat/agent.h>
 #include <udjat/state.h>

 namespace Udjat {

	namespace Process {

 		class UDJAT_API Agent : public Udjat::Abstract::Agent {
		public:
			class Information;
			friend class Information;

			/// @brief Agent state.
			enum State : uint8_t {
				STATE_RUNNING		= 'R',	///< @brief Running.
				STATE_SLEEPING		= 'S',	///< @brief Sleeping in an interruptible wait.
				STATE_WAITING		= 'D',	///< @brief Waiting in uninterruptible disk sleep.
				STATE_ZOMBIE		= 'Z',	///< @brief Zombie.
				STATE_STOPPED		= 'T',	///< @brief Stopped (on a signal) or (before Linux 2.6.33) trace stopped.
				STATE_TRACING_STOP	= 't',	///< @brief Tracing stop (Linux 2.6.33 onward).
				STATE_PAGING		= 'W',	///< @brief Paging (only before Linux 2.6.0).
				STATE_DEAD			= 'X',	///< @brief Dead (from Linux 2.6.0 onward).
				STATE_DEAD_COMPAT	= 'x',	///< @brief Dead (Linux 2.6.33 to 3.13 only).
				STATE_WAKE_KILL		= 'K',	///< @brief Wakekill (Linux 2.6.33 to 3.13 only).
				STATE_WAKING		= 'W',	///< @brief Waking (Linux 2.6.33 to 3.13 only).
				STATE_PARKED		= 'P',	///< @brief Parked (Linux 3.9 to 3.13 only).

				STATE_UNDEFINED		= 0		///< @brief Undefined.
			};

		private:

			class Controller;
			const Information *info = nullptr;

		protected:
			Agent();

		public:

			class Factory : public Udjat::Factory {
			public:
				Factory();
				void parse(Abstract::Agent &parent, const pugi::xml_node &node) const override;
			};

			virtual ~Agent();

 		};

	}

 }

