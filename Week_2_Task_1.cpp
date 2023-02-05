// Arseny Savchenko

#include <memory>
#include <iostream>
#include <sstream>

template <typename T> struct stack {
    virtual std::size_t get_size() const = 0;
    virtual bool is_empty() const = 0;
    virtual void push(T&& elem) = 0;
    virtual void push(T elem) = 0;
    virtual std::shared_ptr<T> pop() = 0;
    virtual std::shared_ptr<T> get_last() const = 0;
};

template <typename T> class linked_stack : public stack<T> {
    struct stack_node {
        std::shared_ptr<T> item;
        std::shared_ptr<stack_node> prev;

        stack_node(std::shared_ptr<T> item, std::shared_ptr<stack_node> prev) {
            this->item = item;

            if (prev != nullptr)
                this->prev = prev;
        }

        ~stack_node() = default;
    };

    std::size_t size = 0;
    std::shared_ptr<stack_node> last;

public:
    linked_stack() = default;
    ~linked_stack() = default;

    std::size_t get_size() const override {
        return size;
    }

    bool is_empty() const override {
        return size == 0;
    }

    void push(T&& item) override {
        stack_node node(std::make_shared<T>(item), last);
        last = std::make_shared<stack_node>(node);
        ++size;
    }

    void push(T item) override {
        stack_node node(std::make_shared<T>(item), last);
        last = std::make_shared<stack_node>(node);
        ++size;
    }

    std::shared_ptr<T> pop() override {
        --size;
        const auto removed = last;
        last = removed->prev;
        return removed->item;
    }

    std::shared_ptr<T> get_last() const override {
        return last->item;
    }
};

namespace {
    const int MINUS_PRIORITY = 0;
    const int PLUS_PRIORITY = 0;
    const int MUL_PRIORITY = 1;
    const int DIV_PRIORITY = 1;
    const int MIN_PRIORITY = 2;
    const int MAX_PRIORITY = 2;
    const int COMMA_PRIORITY = -1;
    const int BRACE_PRIORITY = -2;

    class operation {
        std::string value;
        int priority;

        operation(std::string&& value, const int priority) {
            this->value = value;
            this->priority = priority;
        }

    public:
        ~operation() = default;

        const std::string& get_value() const { return value; }
        int get_priority() const { return priority; }

        static operation parse(const std::string& value) {
            if (value == "-") return operation("-", MINUS_PRIORITY);
            if (value == "+") return operation("+", PLUS_PRIORITY);
            if (value == "*") return operation("*", MUL_PRIORITY);
            if (value == "/") return operation("/", DIV_PRIORITY);
            if (value == "min") return operation("min", MIN_PRIORITY);
            if (value == "max") return operation("max", MAX_PRIORITY);
            if (value == "(") return operation("(", BRACE_PRIORITY);
            if (value == ")") return operation(")", BRACE_PRIORITY);
            if (value == ",") return operation(",", COMMA_PRIORITY);
            throw std::runtime_error("Unknown operator");
        }
    };

    inline bool is_digit(const std::string& s) {
        return s[0] >= '0' && s[0] <= '9';
    }
}

int main() {
    linked_stack<operation> operations;

    std::string whole_input;
    std::getline(std::cin, whole_input);

    std::stringstream ss(whole_input);
    std::string input;

    while (ss >> input) {
        if (is_digit(input)) {
            std::cout << input << ' ';
        } else {
            const auto sign = operation::parse(input);

            if (sign.get_value() == ",") {
                while (operations.get_last()->get_value() != "(")
                    std::cout << operations.pop()->get_value() << ' ';
            } else if (sign.get_value() == ")") {
                while (operations.get_last()->get_value() != "(")
                    std::cout << operations.pop()->get_value() << ' ';

                operations.pop();

                if (!operations.is_empty() && operations.get_last()->get_priority() == 2)
                    std::cout << operations.pop()->get_value() << ' ';
            } else if (sign.get_value() != "(") {
                while (!operations.is_empty() && operations.get_last()->get_priority() >= sign.get_priority()) {
                    auto sig = operations.pop()->get_value();
                    std::cout << sig << ' ';
                }
            }

            if (sign.get_value() != "," && sign.get_value() != ")")
                operations.push(sign);
        }
    }

    while (!operations.is_empty()) {
        const auto sig = operations.pop()->get_value();
        std::cout << sig << ' ';
    }

    return 0;
}
