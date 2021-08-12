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
 #include <udjat/agent.h>
 #include <udjat/state.h>
 #include <udjat/process/identifier.h>

 namespace Udjat {

	namespace Process {

 		class UDJAT_API Agent : public Udjat::Abstract::Agent {
		public:
			class State;

		private:
			friend class Process::Controller;

			const Identifier *pid = nullptr;

			/// @brief Agent states.
			std::vector<std::shared_ptr<State>> states;

		protected:
			Agent();
			Agent(const pugi::xml_node &node);

			void start() override;

			/// @brief Test if the identifier exename match the agent.
			/// @brief exename The identifier exename.
			/// @return true if the identifier match the agent requirements.
			virtual bool probe(const char *exename) const noexcept = 0;

			/// @brief Test if the identifier match the agent.
			/// @param ident A process identifier.
			/// @return true if the identifier match the agent requirements.
			virtual bool probe(const Identifier &ident) const noexcept;

			virtual void setIdentifier(const Identifier *info);

			std::shared_ptr<Abstract::State> stateFromValue() const override;

		public:

			static bool factory(Abstract::Agent &parent, const pugi::xml_node &node);

			virtual ~Agent();

			void get(const Request &request, Response &response) override;

			bool hasStates() const noexcept override;
			void append_state(const pugi::xml_node &node) override;

 		};

 	}

 }

