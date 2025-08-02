#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include "au_utils.h"

using namespace au;

void Logger_Console_Test();
void Logger_Test();
void Logger_Thread_Test();
void StringCVT_Test();
void Color_Test();
void Types_Test();
void Vector_Test();
void Try_Catch_Test();
void FileSystem_Test();
void PTR_TEST();
void Math_Test();
void Thread_Test();
void Thread_Pool_Test();
void INI_TEST();
void Parallel_For_Test();
void Command_Line_TEST();


#define UNIT_TEST(__func) if(__func()) log::console::Debug("test: %s pass", #__func); else log::console::Error("test: %s fail", #__func);

int main()
{
	log::console::console_enable(true);
	
	Try_Catch_Test();

	Logger_Console_Test();

	Logger_Test();

	Logger_Thread_Test();

	StringCVT_Test();

	StringCVT_Test();

	Color_Test();

	Types_Test();

	Vector_Test();

	PTR_TEST();

	Math_Test();

	FileSystem_Test();

	/*Thread_Test();
	
	Thread_Pool_Test();*/

	INI_TEST();

	//Parallel_For_Test();

	Command_Line_TEST();

	au::sleep_ms(10);

	return 0;

}

void Logger_Console_Test()
{
	log::console::Write(log::eLevel::none,"\n========= Logger_Console_Test =========\n");

	time::timer::start();
	printf("printf test\n");
	time::timer::end();
	log::console::Debug(("printf time {} ms"), time::timer::get_duration(time::eformat::millis));

	time::timer::start();
	log::console::Debug(("hello {} {0x} {}"), "12369", 10, 800.0);
	time::timer::end();
	log::console::Debug(("time {} ms"), time::timer::get_duration(time::eformat::millis));
	log::console::Debug(("time {}"), time::timer::get_duration<time::milli>());
	log::console::Debug(("now {} "), au::to_string(time::now()).c_str());
	using namespace std::literals::chrono_literals;
	log::console::Debug(("now {} "), 1h);

	log::console::Write(log::eLevel::info, "test info {}", 123);
	log::console::Write(log::eLevel::err, "test err {}", 123);
	log::console::Write(log::eLevel::warn, "test warn {}", 123);
	log::console::Write(log::eLevel::none, "test none {}", 123);
	log::console::Write(log::eLevel::none, "test none {0x16}", 0x1111111122223333UL);
}

void Logger_Test()
{
	log::console::Write(log::eLevel::none, "\n========= Logger_Test =========\n");

	log::logger a("123", 0);
	a.Write(log::eLevel::debug, "121213123131213");
	a.Write(log::eLevel::debug, "ss");
	a.Write(log::eLevel::debug, "%d", 1000);


	log::logger_map::assign("1", new log::logger("1", 0) );
	//log::logger_map::assign("1", new log::logger("2"));
	log::logger_map::get("1")->Write(log::eLevel::err, "asda");
}

void Logger_Thread_Test()
{
	log::console::Write(log::eLevel::none,"\n========= Logger_Thread_Test =========\n");

	log::logger* test_log_thread = new log::logger_thread("logThread", au::log::eMode::queue, 0);
	
	log::logger_map::assign("logThread", test_log_thread);

	log::logger_map::get("logThread")->Write(au::log::eLevel::warn, "test");

	for(int i=0;i<50;i++)
		log::logger_map::get("logThread")->Write(au::log::eLevel::warn, "test {} ", i);
	
	
}

void StringCVT_Test()
{
	log::console::Write(log::eLevel::none,"\n\n========= StringCVT_Test =========\n");

	auto v = stringcvt::split("1,2,3,4,5,6", ",");
	auto ss = stringcvt::replace(std::string("1 2 3 4 5 6 "), " |1", "");
	auto ss2 = stringcvt::to_upper("aaaaa");
}
void FileSystem_Test()
{
	log::console::Write(log::eLevel::none,"\n========= FileSystem_Test =========\n");

	bool t = au::fs::is_directory("./Log");
	int file_count = au::fs::file_count("./Log");
	auto files = fs::list_files("./Log");

	fs::sort_by_date(files);
	auto files_overdays = fs::check_over_days(files, 30);
}

void Color_Test()
{
	log::console::Write(log::eLevel::none,"\n========= Color_Test =========\n");

	auto color1 = colors::get_normalize(colors::type::White);
	auto color2 = colors::get_8U(colors::type::White);
	auto color3 = colors::get_16U(colors::type::White);
	
	std::cout << "color1: " << color1;
	std::cout << "color2: " << color2;
	std::cout << "color3: " << color3;

	std::cout << colors::type::White;
	std::cout << colors::type::AlizarinCrimson;
	std::cout << colors::type::Cyan;

	std::cout << au::to_string(colors::type::White);
	std::cout << au::to_string(colors::type::AlizarinCrimson);
	std::cout << au::to_string(colors::type::Cyan);

}

void Types_Test()
{
	log::console::Write(log::eLevel::none,"\n========= Types_Test =========\n");

	std::cout << "\n";
	std::cout << "name: " << types::int2::name() << "\n";
	std::cout << "channels: " << types::int2::channels << "\n";
	std::cout << "type: " << typeid(types::int2::type).name() << "\n";
	std::cout << "value type: " << typeid(types::int2::value_type).name() << "\n";

	types::int2 i1(10, 20);
	types::int2 i2{ 20, 30 };
	types::int2 i3 = { 20, 30 };
	types::int2 i4(i1);
	types::int2 i5 = i1;
	types::int2 i6 = i1 * 10;
	types::int2 i7(types::int2(1, 1));
	types::int2 i8(types::float2(1.5f, 2.9f));
	types::float2 f1(1.5f, 2.9f);
	types::int2 i9(f1);

	types::float2 *f2 = new types::float2(1.5f, 2.9f);
	types::int2 i10(*f2);
	types::int2 i11 = *f2;
	types::int2 i12 = types::convert<types::int2>(*f2);
	i1 += 10;
	i1 -= 10;
	i1 *= 10;
	i1 /= 10;
	//i1 /= 0;

	i1 += 10.0f;
	i1 -= 10.0f;
	i1 *= 10.0f;
	i1 /= 10.0f;

	auto io1 = i1 + 10;
	auto io2 = i1 - 10;
	auto io3 = i1 * 10;
	auto io4 = i1 / 10;
	//auto io5 = i1 / 0;
	auto io6 = i1 + 10.0f;
	auto io7 = i1 - 10.0f;
	auto io8 = i1 * 10.0f;
	auto io9 = i1 / 10.0f;

	i6 += i1;
	i6 -= i1;
	i6 *= i1;
	i6 /= i1;

	auto iio1 = i6 + i1;
	auto iio2 = i6 - i1;
	auto iio3 = i6 * i1;
	auto iio4 = i6 / i1;
}

void Vector_Test()
{
	log::console::Write(log::eLevel::none,"\n========= Vector_Test =========\n");

	std::cout << "\n";
	std::cout << "name: " << types::vec3d::name() << "\n";
	std::cout << "channels: " << types::vec3d::channels << "\n";
	std::cout << "type: " << typeid(types::vec3d::type).name() << "\n";
	std::cout << "value type: " << typeid(types::vec3d::value_type).name() << "\n";

	types::vec3d d1(1.0, 2.0, 3.0);
	types::vec3d d2{ 1.0, 2.0, 3.0 };
	types::vec3d d3 = { 1.0, 2.0, 3.0 };
	types::vec3d d4(d1);
	types::vec3d d5 = d1;
	types::vec3d d6 = d1 * 10;
	types::vec3d d7(types::vec3d(1, 1, 1.0));
	types::vec3d d8(types::vec3d(10, 20, 30));
	types::vec3i i1(10, 20, 30);
	types::vec3d d9(i1);

	types::vec3i* i2 = new types::vec3i(10, 20, 30);
	types::vec3d d10(*i2);
	types::vec3d d11 = *i2;
	types::vec3d d12 = types::convert<types::vec3d>(*i2);

	double arr3[3] = { 6, 7, 8 };
	types::vec3d d13(arr3, 3);

	types::vec3d d14(1, 1, 1);
	types::vec3d d15(1, 2, 1);

	auto dot_v = d14.dot(d15);
	auto length = d14.length();
	d14.normalize();
	auto cross_product = d14.cross(d15);

	
	d1 += 10;
	d1 -= 10;
	d1 *= 10;
	d1 /= 10;
	//d1 /= 0;

	d1 += 10.0f;
	d1 -= 10.0f;
	d1 *= 10.0f;
	d1 /= 10.0f;

	auto do1 = d1 + 10;
	auto do2 = d1 - 10;
	auto do3 = d1 * 10;
	auto do4 = d1 / 10;
	//auto do5 = d1 / 0;

	auto do6 = d1 + 10.0f;
	auto do7 = d1 - 10.0f;
	auto do8 = d1 * 10.0f;
	auto do9 = d1 / 10.0f;

	d6 += d1;
	d6 -= d1;
	d6 *= d1;
	d6 /= d1;

	auto ddo1 = d6 + d1;
	auto ddo2 = d6 - d1;
	auto ddo3 = d6 * d1;
	auto ddo4 = d6 / d1;
}

void throw_test()
{
	AU_THROW_ERR("123", exception::etype::DIVIDE_BY_ZERO);
};

void Try_Catch_Test()
{
	log::console::Write(log::eLevel::none,"\n========= Try_Catch_Test =========\n");

	AU_TRY
	{
		throw_test();
	}
	AU_CATCH
	{
		log::console::Debug(("{}"), e.what());
	}

}


void PTR_TEST()
{
	log::console::Write(log::eLevel::none,"\n========= PTR_TEST =========\n");

	using namespace aptr;

	struct ptr_struct {
		ptr_struct() = default;
		ptr_struct(int _a, int _b) : a(_a), b(_b) {}
		int a = 0;
		int b = 0;
		void Release()
		{
			printf("ptr release\n");
		}
	};

	struct parrent {
		
	};

	struct AAA : public parrent {
		AAA() = default;
		AAA(int _a, int _b) : a(_a), b(_b) {}
		int a = 0;
		int b = 0;
	};

	auto ptr_release = [](ptr_struct *p)
	{
		printf("lambda ptr release\n");
	};

	ptr<ptr_struct> test_ptr1 = make_ptr(new ptr_struct(1, 1));

	ptr<ptr_struct> test_ptr2(std::shared_ptr<ptr_struct>(new ptr_struct(3, 3)));

	ptr<ptr_struct> test_ptr3 = make_ptr_release(new ptr_struct(1, 1));

	ptr<ptr_struct> test_ptr4 = std::shared_ptr<ptr_struct>(new ptr_struct(1, 1), ptr_release);
	
	ptr<parrent> ptr_parrent = make_ptr(new AAA(10,20));

	ptr<AAA> pptr_A = ptr_parrent.staticCast<AAA>();

	ptr<parrent> ptr_parrent2 = pptr_A.dynamicCast<parrent>();

	ptr<std::vector<int>> test_vector_ptr = make_ptr(std::vector<int>());

	test_vector_ptr->push_back(10);
	test_vector_ptr->push_back(10);

	uptr<int> test_uptr = std::make_unique<int>(10);

	ptr<int> test_move_ptr = std::move(test_uptr);


}


void Math_Test()
{
	log::console::Write(log::eLevel::none,"\n========= Math_Test =========\n");

	auto deg = math::rad2deg(math::pi);
	auto rad = math::deg2rad(deg);

	types::double2 intersection1;
	types::double2 p1(0, 0);
	types::double2 p2(10, 10);
	types::double2 p3(10, 0);
	types::double2 p4(0, 10);
	bool is_intersection = math::get_2d_intersection_point(p1, p2, p3, p4, &intersection1);

	types::double3 intersection2;
	types::double3 p11(0, 0, 0);
	types::double3 vec1(10, 10, 0);
	types::double3 p22(10, 0, 0);
	types::double3 vec2(-10, 10, 0);

	bool is_sameplane = math::check_if_2_line_same_plane_intersection(p11, vec1, p22, vec2, &intersection2);

	types::double3 p31(1, 2, 3);
	types::double3 p32(10, 10, 0);
	types::double3 p33(10, 0, 0);
	types::double3 p34(0, 10, 0);

	auto closr_point = math::get_3d_closer_point(p31, p32, p33, p34);
}


void Thread_Test()
{
	log::console::Write(log::eLevel::none,"\n========= Thread_Test =========\n");

	thread::IThread *t = new thread::cppThread();

	auto print_job = []()
	{
		static int cnt = 0;
		printf("cnt = %d\n", cnt);
		cnt++;
		au::sleep_ms(1000);

	};

	t->AddJob(print_job);
	t->Start();


	class test_thread : public thread::cppThread
	{
	public:
		test_thread() : cppThread("test_thread")
		{			
			Start();
		}
		void Run() override
		{
			static int cnt = 0;
			printf("test_thread cnt = %d\n", cnt);
			cnt++;
			au::sleep_ms(100);
		}
	};

	test_thread test_thread1;
	printf("thread name: %s\n", test_thread1.ThreadName().c_str());
	au::sleep_ms(1000);
	test_thread1.Terminate();
	t->Terminate();
}

void Thread_Pool_Test()
{
	log::console::Write(log::eLevel::none,"\n========= Thread_Pool_Test =========\n");

	using namespace au::thread;

	class test_Worker1 : public pool::TheadWorker
	{
		void Run() override
		{
			printf("\n\n");
			for (int i = 0; i < 10; i++)
				printf("test_worker1 \n");
			au::sleep_ms(1000);
			printf("test_worker1 finish\n");
		}
	};

	class test_Worker2 : public pool::TheadWorker
	{
		void Run() override
		{
			printf("\n\n");
			printf("test_worker2 in waiting\n");
			au::sleep_ms(2000);
			printf("test_worker2 waiting finished !\n");
		}
	};

	class test_Worker3 : public pool::TheadWorker
	{
		void Run() override
		{
			printf("\n\n");
			for (int i = 0; i < 10; i++)
				printf("test_worker3 \n");
		}
	};

	pool::Immediate::Instance()->Execute(new test_Worker1());
	pool::Immediate::Instance()->Execute(new test_Worker1());

	pool::Sequential::Instance()->Apply({ new test_Worker2(), 1 });
	pool::Sequential::Instance()->Apply({ new test_Worker1(), 2 });
	pool::Sequential::Instance()->Apply({ new test_Worker1(), 2 });
	pool::Sequential::Instance()->Apply({ new test_Worker3(), 3 });
	pool::Sequential::Instance()->Start();

	printf("start\n");
	while (!thread::pool::Sequential::Instance()->IsFinished());
	printf("finished\n");

}

void INI_TEST()
{
	log::console::Write(log::eLevel::none,"\n========= INI_TEST =========\n");

	ini::IniData data = ini::IniFile::Read("123.ini");
	auto b1 = data.HasSection("Test");
	auto b2 = data.HasKey("Test", "Variable");
	auto b3 = data["Test"].HasKey("Variable");

	data["Test"]["Version"] = "1.1.1.1";
	data["Test"]["Variable"] = 10;
	data["Test"]["Variable1"] = 10.0f;
	data["Test"]["Variable2"] = 10.0;
	data["Test"]["Variable3"] = "sss";
	data["Test"]["Variable4"] = true;
	data["Test"]["Variable5"] = std::string("ccc");

	data["Test2"].DeleteKey("Variable");
	data.DeleteSection("Test2");
	ini::IniFile::Write(data);

	ini::IniFile::DirectChange("123.ini", "Test", "Version", "1.1.1.1.2");
	ini::IniFile::DirectChange("123.ini", "Test2", "Version", "1.0.0.0");

	auto v1 = data["Test"]["Version"].as<std::string>("");
	auto v2 = data["Test"]["Variable"].as<int>();
	auto v3 = data["Test"]["Variable1"].as<float>(20.f);
	auto v4 = data["Test"]["Variable2"].as<double>();
	auto v5 = data["Test"]["Variable3"].as<std::string>();
	auto v6 = data["Test"]["Variable4"].as<bool>();

	auto v7 = data["Test"]["Version"].as_split<int>("\\.");
}

void Parallel_For_Test()
{
	log::console::Write(log::eLevel::none, "\n========= Parallel_For_Test =========\n");

	class test_parallel : public au::parallel::For_Base 
	{
	public:
		std::string str;
		test_parallel(std::string _str): str(_str) {}
		~test_parallel() { }
		void operator() (const au::parallel::range& range) override
		{
			printf("%s(%d, %d) = ", str.c_str(), range.start, range.end);
			for (int i = range.start; i < range.end; i++) {
				printf("%d ",i);
			}
			printf("\n");
		}

	};

	au::parallel::FOR_(au::parallel::range(0, 100), test_parallel("test"));
	
	int size = 10000000;
	int* arr = new int[size];
	memset(arr, 0, sizeof(int) * size);

	time::timer::start();
	au::parallel::FOR_(au::parallel::range(0, size), [&](const au::parallel::range& range)
		{
			for (int i = range.start; i < range.end; i++) {
				types::double3 p31(1, 2, 3);
				types::double3 p32(10, 10, 0);
				types::double3 p33(10, 0, 0);
				types::double3 p34(0, 10, 0);
				auto closr_point = math::get_3d_closer_point(p31, p32, p33, p34);
				arr[i] += 1;
			}
		});
	time::timer::end();

	log::console::Debug("parallel time1 = {} ms", time::timer::get_duration(time::eformat::millis));

	time::timer::start();
	for (int i = 0; i < size; i++) {
		types::double3 p31(1, 2, 3);
		types::double3 p32(10, 10, 0);
		types::double3 p33(10, 0, 0);
		types::double3 p34(0, 10, 0);
		auto closr_point = math::get_3d_closer_point(p31, p32, p33, p34);
		arr[i] += 1;
	}
	time::timer::end();

	log::console::Debug("no parallel time2 = {} ms", time::timer::get_duration(time::eformat::millis));

	for (int i = 0; i < size; i++) {
		if (arr[i] != 2) {
			log::console::Debug("parallel for error.");
		}
	}
	delete[] arr;
}

AU_DEFINE_CMD(int, test_int, 10, "command test for int declare");
AU_DEFINE_CMD(double, test_double, 10.0, "command test for double declare");
AU_DEFINE_CMD(bool, test_bool, true, "command test for bool declare");
AU_DEFINE_CMD(string, test_string, "hello", "command test for string declare");

std::string sssss() { return "function string test"; }
AU_DEFINE_CMD(string, test_string2, sssss(), "command test for string declare");

AU_DEFINE_CMD_INFO(h, "show command help");
AU_DEFINE_CMD_INFO(v, "show version");



void Command_Line_TEST()
{
	log::console::Write(log::eLevel::none, "\n========= Command_Line_TEST =========\n");

	int argc = 6;
	char* argv[6];
	argv[0] = (char*)"-test_int";
	argv[1] = (char*)"-test_double 30";
	argv[2] = (char*)"-test_bool false";
	argv[3] = (char*)"--test_string hello sssss string test";
	argv[4] = (char*)"-h";
	argv[5] = (char*)"-v";

	cmdline::Parser(argc, argv);
	log::console::Debug("CMD_test_int = {} ", CMD_test_int);
	log::console::Debug("CMD_test_double = {} ", CMD_test_double);
	log::console::Debug("CMD_test_bool = {} ", CMD_test_bool);
	log::console::Debug("CMD_test_string = {} ", CMD_test_string.c_str());
	log::console::Debug("CMD_test_string2 = {} ", CMD_test_string2.c_str());

	log::console::Debug("CMD_h = {} ", CMD_INFO_h);
	log::console::Debug("CMD_v = {} ", CMD_INFO_v);
	printf("\n");
	if (CMD_INFO_v) {
		printf("%s\n\n", "version 0.0.0.0");
	}
	if (CMD_INFO_h) {
		// you can generate by GenerateHelpString or make by yourself.
		printf("%s\n", cmdline::GenerateHelpString().c_str());
	}
	
	auto v = cmdline::getDefault_int("test_int");
	auto v2 = cmdline::getDefault_string("test_string2");
	
}