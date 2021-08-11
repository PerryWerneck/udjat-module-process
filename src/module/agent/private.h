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
 #include <udjat/defs.h>
 #include <udjat/process/agent.h>

 using namespace std;

 namespace Udjat {

	namespace Process {

		/// @brief Monitor process by exename
		class ExeNameAgent : public Process::Agent {
		private:

			/// @brief The name of the process to monitor.
			const char *exename;

		public:
			ExeNameAgent(const char *exename, const pugi::xml_node &node);

			bool probe(const char *exename) const noexcept override;

		};

	}

 }
