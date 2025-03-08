#pragma once
#include "abstractSub.hpp"
#include "abstractOpt.hpp"
#include <functional>
#include "../exceptionHandler.hpp"
#include "../opt.hpp"
#include "boolOpt.hpp"
#include "numOpt.hpp"
#include "numBoolOpt.hpp"
namespace base::listUiTypes {
	inline bool g_leftPress{};
	inline bool g_rightPress{};
	inline bool g_enterPress{};
	class sub : public abstractSub {
	public:
		void executeOptAdding() {
			__try {
				if (const auto& act = m_action; act) {
					act(this);
				}
			}
			__except (exceptionHandler(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER) {}
			if (m_selectedOption >= m_optionStack.size())
				m_selectedOption = 0;
		}
		void resetOptCount() {
			m_optionStack.clear();
		}
	public:
		abstractOpt* getOpt(size_t opPos) {
			if (opPos > m_optionStack.size())
				return nullptr;
			return m_optionStack[opPos].get();
		}
		template <typename optType>
		void addOption(optType type, bool translate = false) {
			auto opt = new optType(type);
			if (m_name.find("Invalid") == -1 && (m_name.find(".gif") == -1 && m_name.find(".png") == -1 && m_name.find(".jpeg") == -1) && translate && !DISABLE_TRANSLATIONS) {
				auto& sub = m_id.getJson(opt->getDescription());
				if (sub["opName"] != opt->getLeft()) {
					m_id.setData(sub, opt->getLeft(), opt->getDescription());
					if (sub["opTable"][opt->getLeft()]["opName"].is_null() || sub["opTable"][opt->getLeft()]["opName"].empty()) {
						sub["opTable"][opt->getLeft()]["opName"] = opt->getLeft();
						g_config.save();
					}
					else {
						if (opt->getLeft() != sub["opTable"][opt->getLeft()]["opName"].get<std::string>())
							opt->setLeft(sub["opTable"][opt->getLeft()]["opName"].get<std::string>());
					}
					if (sub["opTable"][opt->getLeft()]["opDescription"].is_null() || sub["opTable"][opt->getLeft()]["opDescription"].empty()) {
						sub["opTable"][opt->getLeft()]["opDescription"] = opt->getDescription();
						g_config.save();
					}
					else {
						if (opt->getDescription() != sub["opTable"][opt->getLeft()]["opDescription"].get<std::string>())
							opt->setDesc(sub["opTable"][opt->getLeft()]["opDescription"].get<std::string>());
					}
					if (sub["opTable"][opt->getLeft()]["opType"].is_null() || sub["opTable"][opt->getLeft()]["opType"].empty()) {
						sub["opTable"][opt->getLeft()]["opType"] = opt->getOptionTypeString();
						g_config.save();
					}
					if (!DISBABLE_CONFIG_SECTION) {
						if (opt->getOptionTypeString() == "regularOpt") {
							if (sub["opTable"][opt->getLeft()]["opRight"].is_null() || sub["opTable"][opt->getLeft()]["opRight"].empty()) {
								sub["opTable"][opt->getLeft()]["opRight"] = opt->getRight();
								g_config.save();
							}
							else {
								if (sub["opTable"][opt->getLeft()]["opRight"] != opt->getRight())
									sub["opTable"][opt->getLeft()]["opRight"] = opt->getRight();
								if (opt->getRight() != sub["opTable"][opt->getLeft()]["opRight"].get<std::string>()) {
									g_config.save();
									g_config.load(g_config.m_currentLanguage);
									opt->setRight(sub["opTable"][opt->getLeft()]["opRight"].get<std::string>());
								}
							}
						}
						switch (opt->getOptType()) {
						case eOptType::BoolOpt: {
							if (sub["opTable"][opt->getLeft()]["opToggle"].is_null() || sub["opTable"][opt->getLeft()]["opToggle"].empty()) {
								sub["opTable"][opt->getLeft()]["opToggle"] = reinterpret_cast<boolOpt*>(opt)->getBool();
								g_config.save();
							}
							else if (g_enterPress) {
								if (sub["opTable"][opt->getLeft()]["opToggle"] != reinterpret_cast<boolOpt*>(opt)->getBool())
									sub["opTable"][opt->getLeft()]["opToggle"] = reinterpret_cast<boolOpt*>(opt)->getBool();
								if (reinterpret_cast<boolOpt*>(opt)->getBool() != sub["opTable"][opt->getLeft()]["opToggle"].get<bool>()) {
									g_config.save();
									g_config.load(g_config.m_currentLanguage);
									reinterpret_cast<boolOpt*>(opt)->setBoolValue(sub["opTable"][opt->getLeft()]["opToggle"].get<bool>());
								}
							}
						} break;
						case eOptType::NumOpt: {
							if (std::is_same<decltype(opt), std::add_pointer_t<numOpt<float>>>::value) {
								if (sub["opTable"][opt->getLeft()]["opNumber"].is_null() || sub["opTable"][opt->getLeft()]["opNumber"].empty()) {
									sub["opTable"][opt->getLeft()]["opNumber"] = reinterpret_cast<numOpt<float>*>(opt)->getNum();
								}
								else if (g_leftPress || g_rightPress) {
									if (sub["opTable"][opt->getLeft()]["opNumber"] != reinterpret_cast<numOpt<float>*>(opt)->getNum())
										sub["opTable"][opt->getLeft()]["opNumber"] = reinterpret_cast<numOpt<float>*>(opt)->getNum();
									if (reinterpret_cast<numOpt<float>*>(opt)->getNum() != sub["opTable"][opt->getLeft()]["opNumber"].get<float>()) {
										g_config.save();
										g_config.load(g_config.m_currentLanguage);
										reinterpret_cast<numOpt<float>*>(opt)->setNumValue(sub["opTable"][opt->getLeft()]["opNumber"].get<float>());
									}
								}
							}
							else if (std::is_same<decltype(opt), std::add_pointer_t<numOpt<double>>>::value) {
								if (sub["opTable"][opt->getLeft()]["opNumber"].is_null() || sub["opTable"][opt->getLeft()]["opNumber"].empty()) {
									sub["opTable"][opt->getLeft()]["opNumber"] = reinterpret_cast<numOpt<double>*>(opt)->getNum();
								}
								else if (g_leftPress || g_rightPress) {
									if (sub["opTable"][opt->getLeft()]["opNumber"] != reinterpret_cast<numOpt<double>*>(opt)->getNum())
										sub["opTable"][opt->getLeft()]["opNumber"] = reinterpret_cast<numOpt<double>*>(opt)->getNum();
									if (reinterpret_cast<numOpt<float>*>(opt)->getNum() != sub["opTable"][opt->getLeft()]["opNumber"].get<double>()) {
										g_config.save();
										g_config.load(g_config.m_currentLanguage);
										reinterpret_cast<numOpt<float>*>(opt)->setNumValue(sub["opTable"][opt->getLeft()]["opNumber"].get<double>());
									}
								}
							}
							else if (std::is_same<decltype(opt), std::add_pointer_t<numOpt<int>>>::value || std::is_same<decltype(opt), std::add_pointer_t<numOpt<uint32_t>>>::value) {
								if (sub["opTable"][opt->getLeft()]["opNumber"].is_null() || sub["opTable"][opt->getLeft()]["opNumber"].empty()) {
									sub["opTable"][opt->getLeft()]["opNumber"] = reinterpret_cast<numOpt<int>*>(opt)->getNum();
								}
								else if (g_leftPress || g_rightPress) {
									if (sub["opTable"][opt->getLeft()]["opNumber"] != reinterpret_cast<numOpt<int>*>(opt)->getNum())
										sub["opTable"][opt->getLeft()]["opNumber"] = reinterpret_cast<numOpt<int>*>(opt)->getNum();
									if (reinterpret_cast<numOpt<int>*>(opt)->getNum() != sub["opTable"][opt->getLeft()]["opNumber"].get<int>()) {
										g_config.save();
										g_config.load(g_config.m_currentLanguage);
										reinterpret_cast<numOpt<int>*>(opt)->setNumValue(sub["opTable"][opt->getLeft()]["opNumber"].get<int>());
									}
								}
							}
						} break;
						case eOptType::NumBoolOpt: {
							if (sub["opTable"][opt->getLeft()]["opToggle"].is_null() || sub["opTable"][opt->getLeft()]["opToggle"].empty()) {
								sub["opTable"][opt->getLeft()]["opToggle"] = reinterpret_cast<numBoolOpt<int>*>(opt)->getBool();
								g_config.save();
							}
							else if (g_enterPress) {
								if (sub["opTable"][opt->getLeft()]["opToggle"] != reinterpret_cast<numBoolOpt<int>*>(opt)->getBool())
									sub["opTable"][opt->getLeft()]["opToggle"] = reinterpret_cast<numBoolOpt<int>*>(opt)->getBool();
								if (reinterpret_cast<numBoolOpt<int>*>(opt)->getBool() != sub["opTable"][opt->getLeft()]["opToggle"].get<bool>()) {
									g_config.save();
									g_config.load(g_config.m_currentLanguage);
									reinterpret_cast<numBoolOpt<int>*>(opt)->setBoolValue(sub["opTable"][opt->getLeft()]["opToggle"].get<bool>());
								}
							}
							if (std::is_same<decltype(opt), std::add_pointer_t<numBoolOpt<float>>>::value) {
								if (sub["opTable"][opt->getLeft()]["opNumber"].is_null() || sub["opTable"][opt->getLeft()]["opNumber"].empty()) {
									sub["opTable"][opt->getLeft()]["opNumber"] = reinterpret_cast<numBoolOpt<float>*>(opt)->getNum();
								}
								else if (g_leftPress || g_rightPress) {
									if (sub["opTable"][opt->getLeft()]["opNumber"] != reinterpret_cast<numBoolOpt<float>*>(opt)->getNum())
										sub["opTable"][opt->getLeft()]["opNumber"] = reinterpret_cast<numBoolOpt<float>*>(opt)->getNum();
									if (reinterpret_cast<numBoolOpt<float>*>(opt)->getNum() != sub["opTable"][opt->getLeft()]["opNumber"].get<float>()) {
										g_config.save();
										g_config.load(g_config.m_currentLanguage);
										reinterpret_cast<numBoolOpt<float>*>(opt)->setNumValue(sub["opTable"][opt->getLeft()]["opNumber"].get<float>());
									}
								}
							}
							else if (std::is_same<decltype(opt), std::add_pointer_t<numBoolOpt<double>>>::value) {
								if (sub["opTable"][opt->getLeft()]["opNumber"].is_null() || sub["opTable"][opt->getLeft()]["opNumber"].empty()) {
									sub["opTable"][opt->getLeft()]["opNumber"] = reinterpret_cast<numBoolOpt<double>*>(opt)->getNum();
								}
								else if (g_leftPress || g_rightPress) {
									if (sub["opTable"][opt->getLeft()]["opNumber"] != reinterpret_cast<numBoolOpt<double>*>(opt)->getNum())
										sub["opTable"][opt->getLeft()]["opNumber"] = reinterpret_cast<numBoolOpt<double>*>(opt)->getNum();
									if (reinterpret_cast<numBoolOpt<float>*>(opt)->getNum() != sub["opTable"][opt->getLeft()]["opNumber"].get<double>()) {
										g_config.save();
										g_config.load(g_config.m_currentLanguage);
										reinterpret_cast<numBoolOpt<float>*>(opt)->setNumValue(sub["opTable"][opt->getLeft()]["opNumber"].get<double>());
									}
								}
							}
							else if (std::is_same<decltype(opt), std::add_pointer_t<numBoolOpt<int>>>::value || std::is_same<decltype(opt), std::add_pointer_t<numBoolOpt<uint32_t>>>::value) {
								if (sub["opTable"][opt->getLeft()]["opNumber"].is_null() || sub["opTable"][opt->getLeft()]["opNumber"].empty()) {
									sub["opTable"][opt->getLeft()]["opNumber"] = reinterpret_cast<numBoolOpt<int>*>(opt)->getNum();
								}
								else if (g_leftPress || g_rightPress) {
									if (sub["opTable"][opt->getLeft()]["opNumber"] != reinterpret_cast<numBoolOpt<int>*>(opt)->getNum())
										sub["opTable"][opt->getLeft()]["opNumber"] = reinterpret_cast<numBoolOpt<int>*>(opt)->getNum();
									if (reinterpret_cast<numBoolOpt<int>*>(opt)->getNum() != sub["opTable"][opt->getLeft()]["opNumber"].get<int>()) {
										g_config.save();
										g_config.load(g_config.m_currentLanguage);
										reinterpret_cast<numBoolOpt<int>*>(opt)->setNumValue(sub["opTable"][opt->getLeft()]["opNumber"].get<int>());
									}
								}
							}
						} break;
						}
					}
				}
				else {
					if (sub["opName"].is_null() || sub["opName"].empty()) {
						sub["opName"] = opt->getLeft();
						g_config.save();
					}
					else {
						if (opt->getLeft() != sub["opName"].get<std::string>())
							opt->setLeft(sub["opName"].get<std::string>());
					}
				}
			}
			m_optionStack.emplace_back(std::move(opt));
		}
	public:
		std::string getName() {
			return m_name;
		}
		subId getId() {
			return m_id;
		}
		size_t getNumberOfBreakOptions() {
			size_t numOfBreakOpts{};
			for (auto const& opt : m_optionStack) {
				if (opt->getOptType() == eOptType::BreakOpt)
					numOfBreakOpts++;
			}
			return numOfBreakOpts;
		}
		size_t getNumberOfOptions() {
			return m_optionStack.size();
		}
		size_t getSelectedOpt() {
			return m_selectedOption;
		}
	public:
		void setSelectedOpt(size_t opPos) {
			m_selectedOption = opPos;
			//if (m_optionStack[opPos] && m_optionStack[opPos]->getOptType() == eOptType::BreakOpt)
			//m_selectedOption += 1;
		}
		sub setName(std::string name, bool translate = false) {
			m_name = name;
			if (m_name.find("Invalid") == std::string::npos && translate) {
				auto& sub = m_id.getJson(std::string());
				if (!sub.is_null()) {
					if (m_name != sub["opName"].get<std::string>() && sub["opName"].get<std::string>() != "..")
						m_name = sub["opName"].get<std::string>();
					if (!m_name.compare("Selected")) {
						m_name = name;
						sub["opName"] = std::string("..");
					}
				}
			}
			return *this;
		}
		sub setId(subId id) {
			m_id = id;
			return *this;
		}
		sub setAction(std::function<void(sub*)> submenuAction) {
			m_action = std::move(submenuAction);
			return *this;
		}
	private:
		std::string m_name;
		subId m_id;
		std::function<void(sub*)> m_action;
		std::vector<std::shared_ptr<abstractOpt>> m_optionStack;
		size_t m_selectedOption;
	};
}