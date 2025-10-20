#include <iostream>
#include <fstream>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include "engine/core/game_app.h"

void testSpdlog();
void testJson();

int main(int, char**) {

	spdlog::set_level(spdlog::level::debug);
	engine::core::GameApp app;
	app.run();
    
    // 执行测试spdlog json 库
	{
		// testSpdlog();
		// testJson();
	}

    return 0;
}

void testSpdlog() {
	// 设置日志等级, 不设置默认info
	spdlog::set_level(spdlog::level::trace);

	// 不同等级梯队
	spdlog::trace("最低等级trace!");
	spdlog::debug("调试信息debug!");
	spdlog::info("信息info!");
	spdlog::warn("警告!很可能出错warn!");
	spdlog::error("错误!error!");
	spdlog::critical("最高级别的log, 比error严重!");

	// 格式化输出 类似fmt
	spdlog::info("日志格式化输出: {} {} {}", 1, "第二个占位", 3.000);
}

void testJson() {
	try {
		// 载入json文件
		std::ifstream infile("assets/example.json");
		nlohmann::json data = nlohmann::json::parse(infile);
		infile.close();
		spdlog::info("JSON Load Successfully");

		// 获取不同类型的数据
		// 字符串
		std::string name = data["name"].get<std::string>();
		spdlog::info("name = {}", name);

		// 数字
		int age = data["age"].get<int>();
		double height = data["height_meters"].get<double>();
		spdlog::info("Age: {}, height: {}", age, height);

		// 布尔值
		bool isStudent = data["isStudent"].get<bool>();
		spdlog::info("Is student: {}", isStudent);

		// null
		if (data["middleName"].is_null()) {
			spdlog::info("Middle Name: null");
		}
		else {
			spdlog::info("Middle Name: {}", data["middleName"].get<std::string>());
		}

		// 成员函数at()获取
		std::string email = data.at("email").get<std::string>();
		spdlog::info("Email: {}", email);

		// 安全访问的方法
		// 成员函数contains检查某个键是否存在
		if (data.contains("email")) {
			email = data.at("email").get<std::string>();
			spdlog::info("Email: {}", email);
		}
		if (data.contains("nonExistentKey")) {
			spdlog::info("nonExistentKey found");
		}
		else {
			spdlog::info("nonExistentKey not found");
		}
		
		// 成员函数value获取一个可能存在的值,不存在则返回指定的默认值(第二个参数)
		std::string optionalValue = data.value("optionalKey", "defaultStringValue");
		int optionalInt = data.value("optionalNumber", 42);
		spdlog::info("Optional Key (string): {}", optionalValue);
		spdlog::info("Optional Key (int): {}", optionalInt);

		// 数组 - 字符串数组
		spdlog::info("Hobbies:");
		nlohmann::ordered_json hobbies = data["hobbies"];
		for (const auto& hobby : hobbies) {
			spdlog::info("	- {}", hobby.get<std::string>());
		}

		// 数组 - 数字数组
		spdlog::info("Scores:");
		for (const auto& score : data["scores"]) {
			if (score.is_number_integer()) {
				spdlog::info("	- {} (integer)", score.get<int>());
			}
			else if (score.is_number_float()) {
				spdlog::info("	- {} (float)", score.get<float>());
			}
		}

		// 数组 - 对象数组
		spdlog::info("Projects:");
		nlohmann::ordered_json projects = data["projects"];
		for (const auto& project : projects) {
			std::string projectName = project["projectName"].get<std::string>();
			std::string status = project["status"].get<std::string>();
			double budget = project.value("budget", 0.0);
			bool isActive = project.value("isActive", false);

			spdlog::info(" ProjectName : {}", projectName);
			spdlog::info(" Status : {}", status);
			spdlog::info(" Budget : {}", budget);
			spdlog::info(" Is Active : {}", isActive);

			// 不包含 或者 包含但值为空
			if (!project.contains("deadline") || project["deadline"].is_null()) {
				spdlog::info(" Deadline : null");
			}
			else {
				spdlog::info(" Deadline : {}", project["deadline"].get<std::string>());
			}
			spdlog::info("--------------------------");
		}

		// 直接访问更深层的嵌套对象和数组
		double metadata = data["metadata"]["version"].get<double>();
		spdlog::info("Metadata Version : {}", metadata);
		spdlog::info("Metadata Tags: ");
		for (const auto& tag : data["metadata"]["tags"]) {
			std::string value = tag.get<std::string>();
			spdlog::info("	- {}", value);
		}

		std::ofstream outfile("assets/save_json.json");
		outfile << data.dump(4);
		outfile.close();
		spdlog::info("JSON 数据已保存到文件 assets/save_json.json");
	}
	catch (const std::exception& e) {
		spdlog::error("Exception : {}", e.what());
	}
}
