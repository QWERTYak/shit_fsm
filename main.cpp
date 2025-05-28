#include <algorithm>
#include <functional>
#include <map>
#include <print>
#include <ranges>
#include <utility>
#include <thread>
#include <fcntl.h>
using namespace std;

template <class TargetType>
/**
 * Warning!Warning!Warning!Warning!Warning!Warning!Warning!Warning!Warning!Warning!
 * This class is not thread safe;
 *
 *
 *
 * 我不知道我不能去说我不知道我不能去说我不知道我不能去说我不知道我不能去说我不知道我不能去说我不知道我不能去说我不知道我不能去说我不知道我不能去说我不知道我不能去说
 * 我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能我不能
 */
class finite_state_machine
{
  public:
    using state_type = std::string; // just identifier
    using finite_state_machine_context_type = finite_state_machine<TargetType> *;
    using event_trigger_type = std::function<bool(finite_state_machine_context_type)>; // bool代表trigger是否会触发
    using action_type = std::function<std::string(
        finite_state_machine_context_type)>; // string代表执行action和state transition后 应该设置的状态列表
                                             // TargetType*代表目标（状态机总要有处理目标吧）
    finite_state_machine(TargetType _target, std::initializer_list<state_type> _state_list)
        : state_list(_state_list), current_state(this->state_list.at(0)), target(_target) {};
    finite_state_machine(const finite_state_machine &) = delete("doesn't support copy constructor");
    finite_state_machine(finite_state_machine &&) = delete("doesn't support moving constructor");
    finite_state_machine operator=(const finite_state_machine &) = delete("doesn't support copy assignment");
    finite_state_machine operator=(const finite_state_machine &&) = delete("doesn't support moving assignment");

    // if state already exists in the state_list,returns error
    //  0: ok, otherwise fail to register a state
    int register_a_state(state_type &_state) // for glvalue
    {
        // is the _state is unique?
        if (this->is_in_state_list(_state))
            return -1;
        state_list.push_back(_state);
        return 0;
    };
    int register_a_state(state_type &&_state) // 让vector重载 for xvalue
    {
        // is the _state is unique?
        if (this->is_in_state_list(_state))
            return -1;
        state_list.emplace_back(_state);
        return 0;
    };
    finite_state_machine &register_a_event_and_action(event_trigger_type &&event_trigger_function,
                                                      action_type &&action_function)
    {
        this->event_triggers_and_actions.emplace_back(std::forward_as_tuple(event_trigger_function, action_function));
        return *this;
    };
    finite_state_machine &register_a_event_and_action(event_trigger_type &event_trigger_function,
                                                      action_type &action_function)
    {
        this->event_triggers_and_actions.push_back(std::make_tuple(event_trigger_function, action_function));
        return *this;
    };
    // 循环执行
    void process()
    {
        if (event_triggers_and_actions.empty() | state_list.empty())
        {
            throw std::runtime_error("empty processing list");
        }
        for (;;)
        {
            for (const event_trigger_and_action_type &item : this->event_triggers_and_actions)
            {
                if (current_state <=> std::string("end") == std::strong_ordering::equal)
                {
                    return;
                }
                if (!std::get<0>(item)(this))
                {
                    continue;
                }
                std::string &&state = std::get<1>(item)(this);
                if (!this->is_in_state_list(state))
                {
                    throw std::runtime_error("illegal state!");
                }
                continue;
            }
        }
    };
    state_type &get_current_state()
    {
        return this->current_state;
    };
    TargetType &get_target()
    {
        return this->target;
    }
    bool is_in_state_list(const state_type &state)
    {
        auto is_unique = [&](state_type &_state) {
            if (_state <=> state == std::strong_ordering::equal)
            {
                return true;
            }
            return false;
        };
        return std::ranges::any_of(state_list, is_unique);
    }

  public:
    std::vector<state_type> state_list;
    state_type current_state;
    using event_trigger_and_action_type = std::tuple<event_trigger_type, action_type>;
    std::vector<event_trigger_and_action_type> event_triggers_and_actions;
    TargetType target;
};

class demo
{
  public:
    int atoi_demo(string s) // std::string is for acsii char...
    {
        if (s.empty())
        {
            return 0;
        }
        bool is_minus = false;
        // a map of char converting to signed int
        std::map<char, int> mapping_to_char_conv_int{{'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5},
                                         {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9}, {'0', 0}};
        if (*s.begin() == '-')
        {
            is_minus = true;
            s.erase(0, 1);
        }
        else if (*s.begin() == '+')
        {
            is_minus = false;
            s.erase(0, 1);
        }

    final_transfering:
        // next step
        //  core : converting function: finite state machine

        // std::pair<std::string,int1>,int2>
        // std::string代表输入的待转换字符串
        // int1 代表最终输出的atoi结果
        // int代表临时的进位变量
        finite_state_machine<std::tuple<std::pair<std::string, int>, int>> fsm{forward_as_tuple(make_pair(s, 0), 1),
                                                                               {"processing", "carring", "end"}};
        // event_trigger & action
        fsm.register_a_event_and_action(std::bind(
                                            [&](decltype(fsm)::finite_state_machine_context_type this_ptr) {
                                                if (this_ptr->current_state <=> "processing" ==
                                                    std::strong_ordering::equal)
                                                    return false;
                                                return true;
                                            },
                                            &fsm),
                                        std::bind(
                                            [&](decltype(fsm)::finite_state_machine_context_type this_ptr) {
                                                std::get<1>(this_ptr->target) *= 10;
                                                this_ptr->current_state = std::string("processing");
                                                return std::string("processing");
                                            },
                                            &fsm))
            .register_a_event_and_action(
                std::bind(
                    [&](decltype(fsm)::finite_state_machine_context_type this_ptr) {
                        if (this_ptr->current_state <=> "carring" == std::strong_ordering::equal)
                            return false;
                        return true;
                    },
                    &fsm),
                std::bind(
                    [&](decltype(fsm)::finite_state_machine_context_type this_ptr) {
                        std::string &input_str = std::get<0>(std::get<0>(this_ptr->target));
                        int &output_num = std::get<1>(std::get<0>(this_ptr->target));
                        int &temporary_digit = std::get<1>(this_ptr->target);
                        if (input_str.empty())
                        {
                            this_ptr->current_state = "end";
                            return std::string("end");
                        }
                        // 由于数字都是从大到小 所以越后面越小 所以这里需要使用size()-1
                        if (const auto &accesser = mapping_to_char_conv_int.find(input_str.at(input_str.size() - 1));
                            accesser != mapping_to_char_conv_int.end())
                        {
                            //                     ->second代表这个东西是字符对应的数字 乘上指数
                            output_num += accesser->second * temporary_digit;
                        }
                        else
                        {
                            throw runtime_error("the mapping doesn't exist");
                        }
                        input_str.pop_back();
                        this_ptr->current_state = "carring";
                        return std::string("carring");
                    },
                    &fsm));
        fsm.process();
        std::negate<int> neg;
        return is_minus ? neg(std::get<1>(std::get<0>(fsm.target ))) : std::get<1>(std::get<0>(fsm.target));
    }
};

int main()
{
    return 0;
}
