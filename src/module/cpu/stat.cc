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

 // https://supportcenter.checkpoint.com/supportcenter/portal?eventSubmit_doGoviewsolutiondetails=&solutionid=sk65143
 // https://unix.stackexchange.com/questions/21543/get-cpu-utilization-stats-from-c-program
 // https://stackoverflow.com/questions/16011677/calculating-cpu-usage-using-proc-files
 // https://medium.com/@yogita088/how-to-calculate-cpu-usage-proc-stat-vs-top-e74f99f02d08

 #include <config.h>
 #include <string>
 #include <udjat/process/agent.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <unistd.h>

 using namespace std;

 namespace Udjat {

	CPU::Stat::Stat() {

		char buffer[4096];

		int fd = open("/proc/stat",O_RDONLY);
		if(fd <  0) {

			if(errno == ENOENT) {
				return;
			}

			throw std::system_error(errno, std::system_category(), "Can't open /proc/pid/stat");
		}

		int szBuffer = read(fd,buffer,sizeof(buffer));

		::close(fd);

		if(szBuffer < 1) {
			throw std::system_error(errno, std::system_category(), "Can't read /proc/pid/stat");
		}

		buffer[szBuffer] = 0;

		ssize_t sz = sscanf(
			buffer,
			"%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
			&user,
			&nice,
			&system,
			&idle,
			&iowait,
			&irq,
			&softirq,
			&steal,
			&guest,
			&guest_nice
		);

		if(sz != 10) {
			throw runtime_error("Format error on /proc/stat");
		}

	}


 }

