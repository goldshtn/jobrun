#include <Windows.h>

#include <bitset>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

using namespace std::string_literals;

class arguments
{
private:
	std::optional<size_t> job_commit_megabytes_;
	std::optional<size_t> process_commit_megabytes_;
	std::optional<size_t> process_ws_megabytes_;
	std::optional<size_t> cpu_seconds_;
	std::optional<unsigned int> num_procs_;
	std::optional<bool> breakaway_;
	std::optional<size_t> affinity_;
	std::optional<unsigned int> priority_;
	std::optional<unsigned int> sched_class_;
	std::wstring application_;
	int argc_;
	wchar_t** argv_;
	int current_;
public:
	arguments(int argc, wchar_t** argv)
		: argc_(argc), argv_(argv)
	{
		if (argc < 2)
			usage();

		parse();
	}
	std::optional<size_t> const& job_commit_megabytes() const { return job_commit_megabytes_; }
	std::optional<size_t> const& process_commit_megabytes() const { return process_commit_megabytes_; }
	std::optional<size_t> const& process_ws_megabytes() const { return process_ws_megabytes_; }
	std::optional<size_t> const& cpu_seconds() const { return cpu_seconds_; }
	std::optional<unsigned int> const& num_procs() const { return num_procs_; }
	std::optional<bool> const& breakaway() const { return breakaway_; }
	std::optional<size_t> const& affinity() const { return affinity_; }
	std::optional<unsigned int> const& priority() const { return priority_; }
	std::optional<unsigned int> const& sched_class() const { return sched_class_; }
	std::wstring application() const { return application_; }
private:
	void parse()
	{
		for (current_ = 1; current_ < argc_; ++current_)
		{
			if (argv_[current_] == L"-M"s)
			{
				job_commit_megabytes_ = parse_uint();
			}
			else if (argv_[current_] == L"-m"s)
			{
				process_commit_megabytes_ = parse_uint();
			}
			else if (argv_[current_] == L"-w"s)
			{
				process_ws_megabytes_ = parse_uint();
			}
			else if (argv_[current_] == L"-c"s)
			{
				cpu_seconds_ = parse_uint();
			}
			else if (argv_[current_] == L"-n"s)
			{
				num_procs_ = static_cast<unsigned int>(parse_uint());
			}
			else if (argv_[current_] == L"-b"s)
			{
				breakaway_ = parse_bool();
			}
			else if (argv_[current_] == L"-a"s)
			{
				affinity_ = parse_uint();
			}
			else if (argv_[current_] == L"-p"s)
			{
				priority_ = static_cast<unsigned int>(parse_uint());
			}
			else if (argv_[current_] == L"-s"s)
			{
				sched_class_ = static_cast<unsigned int>(parse_uint());
				if (sched_class_.value() < 0 || sched_class_.value() > 9)
				{
					std::cout << "Scheduling class value must be 0-9, but got: " << sched_class_.value() << '\n';
					usage();
				}
			}
			else
			{
				application_ = argv_[current_];
			}
		}
		if (application_.empty())
		{
			std::cout << "Expected: application name\n";
			usage();
		}
	}
	size_t parse_uint()
	{
		if (current_ == argc_ - 1)
		{
			std::cout << "Expected: integer value\n";
			usage();
		}
		++current_;
		auto value = std::wcstoul(argv_[current_], nullptr, 10);
		if (value == 0)
		{
			std::cout << "Expected: integer value, found '" << argv_[current_] << "'\n";
			usage();
		}
		return value;
	}
	bool parse_bool()
	{
		if (current_ == argc_ - 1)
		{
			std::cout << "Expected: Boolean value (\"yes\" or \"no\")\n";
			usage();
		}
		++current_;
		if (argv_[current_] == L"yes"s)
		{
			return true;
		}
		else if (argv_[current_] == L"no"s)
		{
			return false;
		}
		else
		{
			std::cout << "Expected: Boolean value (\"yes\" or \"no\"), found '" << argv_[current_] << "'\n";
			usage();
		}
	}
	[[noreturn]] void usage()
	{
		std::cout << '\n';
		std::cout << "jobrun - run a process inside a job and limit its behavior\n";
		std::cout << "         copyright (C) 2017 Sasha Goldshtein\n";
		std::cout << '\n';
		std::cout << "USAGE: jobrun [-M MEGABYTES] [-m MEGABYTES] [-w MEGABYTES] [-c SECONDS]\n";
		std::cout << "              [-n NUMPROCS] [-b yes|no] [-a AFFINITY] [-p PRIORITY]\n";
		std::cout << "              [-s SCHEDCLASS] <application>\n";
		std::cout << '\n';
		std::cout << "  -M MEGABYTES   Limit the total committed memory of the job's processes\n";
		std::cout << "  -m MEGABYTES   Limit the committed memory of each of the job's processes\n";
		std::cout << "  -w MEGABYTES   Limit the process working set of each of the job's processes (soft limit)\n";
		std::cout << "  -c SECONDS     Limit the total CPU time of the job's processes\n";
		std::cout << "  -n NUMPROCS    Limit the number of processes in the job\n";
		std::cout << "  -b yes|no      Allow job processes to break away\n";
		std::cout << "  -a AFFINITY    Set the processor affinity of the job's processes\n";
		std::cout << "  -p PRIORITY    Set the priority class of the job's processes\n";
		std::cout << "  -s SCHEDCLASS  Set the scheduling class (0-9) of the job's processes\n";
		// TODO: CPU rate limiting
		// TODO: UI restrictions
		std::cout << '\n';
		std::exit(1);
	}
};

std::wostream& operator<<(std::wostream& os, arguments const& args)
{
	os << "Launching application '" << args.application() << "'\n";
	if (args.job_commit_megabytes())
		os << "  with committed memory limit of " << args.job_commit_megabytes().value() << "MB\n";
	if (args.process_commit_megabytes())
		os << "  with committed memory limit of " << args.process_commit_megabytes().value() << "MB\n";
	if (args.process_ws_megabytes())
		os << "  with working set memory limit of " << args.process_ws_megabytes().value() << "MB\n";
	if (args.cpu_seconds())
		os << "  with CPU limit of " << args.cpu_seconds().value() << " seconds\n";
	if (args.num_procs())
		os << "  with maximum of " << args.num_procs().value() << " active processes\n";
	os << "  " << (args.breakaway() ? "allowing" : "not allowing") << " breakaway\n";
	if (args.affinity())
	{
		auto affinity = args.affinity().value();
		os << "  with processor affinity of " << std::bitset<8 * sizeof(affinity)>(affinity) << '\n';
	}
	if (args.priority())
		os << "  with priority class of " << args.priority().value() << '\n';
	if (args.sched_class())
		os << "  with scheduling class of " << args.sched_class().value() << '\n';
	return os;
}

class job_exception : public std::exception
{
private:
	DWORD error_;
public:
	job_exception(char const* message) : job_exception(message, GetLastError())
	{
	}
	job_exception(char const* message, DWORD error) : std::exception(message), error_(error)
	{
	}
	DWORD error() const { return error_; }
};

class job
{
private:
	HANDLE job_;
public:
	job()
	{
		job_ = CreateJobObject(nullptr, nullptr);
		if (job_ == nullptr)
			throw job_exception("CreateJobObject failed");

		JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = { 0 };
		info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;	// TODO Doesn't seem to work
		if (FALSE == SetInformationJobObject(job_, JobObjectExtendedLimitInformation, &info, sizeof(info)))
			throw job_exception("SetInformationJobObject failed when setting JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE");
	}
	void set_job_commit_limit(size_t megabytes)
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = { 0 };
		info.JobMemoryLimit = megabytes * 1048576;
		info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_JOB_MEMORY;
		if (FALSE == SetInformationJobObject(job_, JobObjectExtendedLimitInformation, &info, sizeof(info)))
			throw job_exception("SetInformationJobObject failed when setting JOB_OBJECT_LIMIT_JOB_MEMORY");
	}
	void set_process_commit_limit(size_t megabytes)
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = { 0 };
		info.ProcessMemoryLimit = megabytes * 1048576;
		info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_MEMORY;
		if (FALSE == SetInformationJobObject(job_, JobObjectExtendedLimitInformation, &info, sizeof(info)))
			throw job_exception("SetInformationJobObject failed when setting JOB_OBJECT_LIMIT_PROCESS_MEMORY");
	}
	void set_ws_limit(size_t megabytes)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION info = { 0 };
		info.MinimumWorkingSetSize = (megabytes / 2) * 1048576;
		info.MaximumWorkingSetSize = megabytes * 1048576;
		info.LimitFlags = JOB_OBJECT_LIMIT_WORKINGSET;
		if (FALSE == SetInformationJobObject(job_, JobObjectBasicLimitInformation, &info, sizeof(info)))
			throw job_exception("SetInformationJobObject failed when setting JOB_OBJECT_LIMIT_WORKINGSET");
	}
	void set_cpu(size_t seconds)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION info = { 0 };
		info.PerJobUserTimeLimit.QuadPart = seconds * 10000000;
		info.LimitFlags = JOB_OBJECT_LIMIT_JOB_TIME;
		if (FALSE == SetInformationJobObject(job_, JobObjectBasicLimitInformation, &info, sizeof(info)))
			throw job_exception("SetInformationJobObject failed when setting JOB_OBJECT_LIMIT_JOB_TIME");
	}
	void set_num_procs(unsigned int num_procs)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION info = { 0 };
		info.ActiveProcessLimit = num_procs;
		info.LimitFlags = JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
		if (FALSE == SetInformationJobObject(job_, JobObjectBasicLimitInformation, &info, sizeof(info)))
			throw job_exception("SetInformationJobObject failed when setting JOB_OBJECT_LIMIT_ACTIVE_PROCESS");
	}
	void set_breakaway(bool breakaway)
	{
		if (breakaway)
		{
			JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = { 0 };
			info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_BREAKAWAY_OK;
			if (FALSE == SetInformationJobObject(job_, JobObjectExtendedLimitInformation, &info, sizeof(info)))
				throw job_exception("SetInformationJobObject failed when setting JOB_OBJECT_LIMIT_BREAKAWAY_OK");
		}
	}
	void set_affinity(size_t affinity)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION info = { 0 };
		info.Affinity = affinity;
		info.LimitFlags = JOB_OBJECT_LIMIT_AFFINITY;
		if (FALSE == SetInformationJobObject(job_, JobObjectBasicLimitInformation, &info, sizeof(info)))
			throw job_exception("SetInformationJobObject failed when setting JOB_OBJECT_LIMIT_AFFINITY");
	}
	void set_priority(unsigned int priority)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION info = { 0 };
		info.PriorityClass = priority;
		info.LimitFlags = JOB_OBJECT_LIMIT_PRIORITY_CLASS;
		if (FALSE == SetInformationJobObject(job_, JobObjectBasicLimitInformation, &info, sizeof(info)))
			throw job_exception("SetInformationJobObject failed when setting JOB_OBJECT_LIMIT_PRIORITY_CLASS");
	}
	void set_sched_class(int sched_class)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION info = { 0 };
		info.SchedulingClass = static_cast<DWORD>(sched_class);
		info.LimitFlags = JOB_OBJECT_LIMIT_SCHEDULING_CLASS;
		if (FALSE == SetInformationJobObject(job_, JobObjectBasicLimitInformation, &info, sizeof(info)))
			throw job_exception("SetInformationJobObject failed when setting JOB_OBJECT_LIMIT_SCHEDULING_CLASS");
	}
	void run_process_in_job(std::wstring const& application)
	{
		std::unique_ptr<wchar_t[]> cmd(new wchar_t[application.size() + 1]);
		wcscpy_s(cmd.get(), application.size() + 1, application.c_str());
		STARTUPINFO si = { 0 };
		PROCESS_INFORMATION pi = { 0 };
		if (FALSE == CreateProcess(nullptr, cmd.get(), nullptr, nullptr, FALSE,
			CREATE_NEW_CONSOLE | CREATE_SUSPENDED, nullptr, nullptr, &si, &pi))
		{
			throw job_exception("CreateProcess failed");
		}
		if (FALSE == AssignProcessToJobObject(job_, pi.hProcess))
			throw job_exception("AssignProcessToJobObject failed");
		if (static_cast<DWORD>(-1) == ResumeThread(pi.hThread))
			throw job_exception("ResumeThread failed");
		CloseHandle(pi.hProcess);
	}
	~job()
	{
		CloseHandle(job_);
	}
};

int wmain(int argc, wchar_t* argv[])
{
	arguments args(argc, argv);
	std::wcout << args;

	try
	{
		job job;
		if (args.job_commit_megabytes())
			job.set_job_commit_limit(args.job_commit_megabytes().value());
		if (args.process_commit_megabytes())
			job.set_process_commit_limit(args.process_commit_megabytes().value());
		if (args.process_ws_megabytes())
			job.set_ws_limit(args.process_ws_megabytes().value());
		if (args.cpu_seconds())
			job.set_cpu(args.cpu_seconds().value());
		if (args.num_procs())
			job.set_num_procs(args.num_procs().value());
		if (args.breakaway())
			job.set_breakaway(args.breakaway().value());
		if (args.affinity())
			job.set_affinity(args.affinity().value());
		if (args.priority())
			job.set_priority(args.priority().value());
		if (args.sched_class())
			job.set_sched_class(args.sched_class().value());
		job.run_process_in_job(args.application());

		std::cout << "Press ENTER to exit the job\n";
		std::cin.get();
	}
	catch (job_exception& je)
	{
		std::cout << je.what() << " with error code: " << je.error() << '\n';
	}

	return 0;
}