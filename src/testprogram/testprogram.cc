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

 #include <udjat/module.h>
 #include <unistd.h>
 #include <udjat/tools/mainloop.h>
 #include <udjat/tools/url.h>

 using namespace std;
 using namespace Udjat;

 int main(int argc, char **argv) {

	setlocale( LC_ALL, "" );

	Logger::redirect(true);

	auto module = udjat_module_init();
	Udjat::load("./test.xml");

	if(Module::find("httpd")) {

		cout << "http://localhost:8989/api/1.0/info/modules.xml" << endl;
		cout << "http://localhost:8989/api/1.0/info/protocols.xml" << endl;

		for(auto child : *Abstract::Agent::root()) {
			cout << "http://localhost:8989/api/1.0/agent/" << child->name() << ".xml" << endl;
		}

	}

	cout << "Waiting for requests" << endl;

	Udjat::MainLoop::getInstance().run();

	Abstract::Agent::deinit();

	cout << "Removing module" << endl;
	delete module;
	Module::unload();

	return 0;
}
