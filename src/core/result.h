#pragma once

#include <QString>
#include <utility>
#include <variant>

namespace quickdeck {

class Error {
public:
    explicit Error(QString message) : message_(std::move(message)) {}

    [[nodiscard]] const QString &message() const { return message_; }

private:
    QString message_;
};

template <typename T>
class Result {
public:
    static Result ok(T value) { return Result(std::move(value)); }

    static Result fail(QString error) { return Result(Error{std::move(error)}); }

    [[nodiscard]] bool is_ok() const { return std::holds_alternative<T>(data_); }

    [[nodiscard]] bool is_err() const { return !is_ok(); }

    [[nodiscard]] const T &value() const { return std::get<T>(data_); }

    [[nodiscard]] T take_value() { return std::move(std::get<T>(data_)); }

    [[nodiscard]] const QString &error() const { return std::get<Error>(data_).message(); }

private:
    explicit Result(T value) : data_(std::move(value)) {}
    explicit Result(Error error) : data_(std::move(error)) {}

    std::variant<T, Error> data_;
};

template <>
class Result<void> {
public:
    static Result ok() { return Result(true, QString()); }

    static Result fail(QString error) { return Result(false, std::move(error)); }

    [[nodiscard]] bool is_ok() const { return ok_; }

    [[nodiscard]] bool is_err() const { return !ok_; }

    [[nodiscard]] const QString &error() const { return error_; }

private:
    Result(bool ok, QString error) : ok_(ok), error_(std::move(error)) {}

    bool ok_;
    QString error_;
};

} // namespace quickdeck
