#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE UtilsTest

#include <iostream>
#include <fstream>

#include <boost/test/unit_test.hpp>

#include "Logger.h"
#include "Employee.h"
#include "Utils.h"

/// Оборачивание строки именем функции, из которой вызвана
#define WRAP(s) (std::string(boost::core::demangle(typeid(*this).name())) + "::" + std::string(__FUNCTION__) + "(): " + (s))
#define PRINT(s) std::cout << (s) << std::endl

class Class
{
public:
    std::string getPosition()
    {
        return position;
    }

    std::string getSurname()
    {
        return surname;
    }

    std::string getName()
    {
        return name;
    }

    std::string getPatronymic()
    {
        return patronymic;
    }

private:
    std::string position = "Должность";
    std::string surname = "Фамилия";
    std::string name = "Имя";
    std::string patronymic = "Отчество";
};

struct ClassFixture
{
    ClassFixture()
    {
        object = new Class();
    }

    ~ClassFixture()
    {
        delete object;
    }

    Class *object;

};

struct LogToFile
{
    LogToFile()
    {
        logFile.open(logFileName.c_str(), std::ios_base::out);
        boost::unit_test::unit_test_log.set_stream(logFile);
        boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_all_errors);
        boost::unit_test::unit_test_log.set_format(boost::unit_test::OF_CLF);
    }

    ~LogToFile()
    {
        boost::unit_test::unit_test_log.test_finish();
        logFile.close();
        logFile.open(logFileName, std::ios_base::in);
        std::stringstream strStream;
        strStream << logFile.rdbuf();
        std::cout << strStream.str() << std::endl;
    }

    std::string logFileName = "errors.txt";
    std::fstream logFile;
};

BOOST_GLOBAL_FIXTURE(LogToFile);

BOOST_AUTO_TEST_SUITE(ClassTest)

BOOST_FIXTURE_TEST_CASE(LOGIN, ClassFixture)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_NO_THROW(LOGIN(object));
}

BOOST_FIXTURE_TEST_CASE(LOGOUT, ClassFixture)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_NO_THROW(LOGOUT(object));
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(UtilsTest)

BOOST_AUTO_TEST_CASE(translitOne)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::translit(""), "");
}

BOOST_AUTO_TEST_CASE(translitTwo)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::translit(" "), " ");
}

BOOST_AUTO_TEST_CASE(translitThree)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::translit("`~!@#$%^&*()_+{}:<>?/|\"№;-=,'.[]\\1234567890"
                                      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"), "");
}

BOOST_AUTO_TEST_CASE(translitFour)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::translit("AБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"), "");
}

BOOST_AUTO_TEST_CASE(translitFive)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::translit("абвгдеёжзийклмнопрстуфхцчшщъыьэюя"),
                                      "abvgdeezhzijklmnoprstufhtschshschjejuja");
}

BOOST_AUTO_TEST_CASE(translitSix)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::translit(" 0Ая "), " ja ");
}


BOOST_AUTO_TEST_CASE(createEmailOne)
{
    PRINT(WRAP("..."));

    const std::vector<std::string> anthroponym = {"surname"};

    BOOST_CHECK(utils::createEmail(anthroponym) != "surname@tradingcompany.ru");
}

BOOST_AUTO_TEST_CASE(createEmailTwo)
{
    PRINT(WRAP("..."));

    const std::vector<std::string> anthroponym = {"surname", "name", "patronymic"};

    BOOST_CHECK(utils::createEmail(anthroponym) != "surname.name.patronymic@tradingcompany.ru");
}

BOOST_AUTO_TEST_CASE(createEmailThree)
{
    PRINT(WRAP("..."));

    const std::vector<std::string> anthroponym = {"фамилия"};

    BOOST_CHECK_EQUAL(utils::createEmail(anthroponym), "familija@tradingcompany.ru");
}

BOOST_AUTO_TEST_CASE(createEmailFour)
{
    PRINT(WRAP("..."));

    const std::vector<std::string> anthroponym = {"фамилия", "имя", "отчество"};

    BOOST_CHECK_EQUAL(utils::createEmail(anthroponym), "familija.imja.otchestvo@tradingcompany.ru");
}


BOOST_AUTO_TEST_CASE(hashOne)
{
    PRINT(WRAP("..."));

    BOOST_CHECK(utils::hash("0") == utils::hash("0"));
}

BOOST_AUTO_TEST_CASE(hashTwo)
{
    PRINT(WRAP("..."));

    BOOST_CHECK(utils::hash("01") == utils::hash("01"));
}

BOOST_AUTO_TEST_CASE(hashThree)
{
    PRINT(WRAP("..."));

    BOOST_CHECK(utils::hash("01") != utils::hash("10"));
}

BOOST_AUTO_TEST_CASE(hashFour)
{
    PRINT(WRAP("..."));

    BOOST_CHECK(utils::hash("b") == utils::hash("b"));
}

BOOST_AUTO_TEST_CASE(hashFive)
{
    PRINT(WRAP("..."));

    BOOST_CHECK(utils::hash("back") == utils::hash("back"));
}


BOOST_AUTO_TEST_CASE(toLowerOne)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toLower("test"), "test");
}

BOOST_AUTO_TEST_CASE(toLowerTwo)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toLower("Test"), "test");
}

BOOST_AUTO_TEST_CASE(toLowerThree)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toLower("Test"), "test");
}

BOOST_AUTO_TEST_CASE(toLowerFour)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toLower("TesT"), "test");
}

BOOST_AUTO_TEST_CASE(toLowerFive)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toLower("TEST"), "test");
}

BOOST_AUTO_TEST_CASE(toLowerSix)
{
    PRINT(WRAP("..."));

    std::string source = "`~!@#$%^&*()_+{}:<>?/|\"№;-=,'.[]\\1234567890"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string result = "`~!@#$%^&*()_+{}:<>?/|\"№;-=,'.[]\\1234567890"
                         "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";

    BOOST_CHECK_EQUAL(utils::toLower(source), result);
}


BOOST_AUTO_TEST_CASE(tolowerOne)
{
    PRINT(WRAP("..."));

    std::string source, result;
    source = result = "test";

    BOOST_CHECK_NO_THROW(utils::tolower(source));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(tolowerTwo)
{
    PRINT(WRAP("..."));

    std::string source = "Test";
    std::string result = "test";

    BOOST_CHECK_NO_THROW(utils::tolower(source));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(tolowerThree)
{
    PRINT(WRAP("..."));

    std::string source = "tesT";
    std::string result = "test";

    BOOST_CHECK_NO_THROW(utils::tolower(source));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(tolowerFour)
{
    PRINT(WRAP("..."));

    std::string source = "TesT";
    std::string result = "test";

    BOOST_CHECK_NO_THROW(utils::tolower(source));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(tolowerFive)
{
    PRINT(WRAP("..."));

    std::string source = "TEST";
    std::string result = "test";

    BOOST_CHECK_NO_THROW(utils::tolower(source));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(tolowerSix)
{
    PRINT(WRAP("..."));

    std::string source = "`~!@#$%^&*()_+{}:<>?/|\"№;-=,'.[]\\1234567890"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string result = "`~!@#$%^&*()_+{}:<>?/|\"№;-=,'.[]\\1234567890"
                         "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";

    BOOST_CHECK_NO_THROW(utils::tolower(source));
    BOOST_CHECK(source == result);
}


BOOST_AUTO_TEST_CASE(splitStringOne)
{
    PRINT(WRAP("..."));

    std::string source = "";
    std::vector<std::string> result = {""};

    BOOST_CHECK(utils::splitString(source, "") == result);
}

BOOST_AUTO_TEST_CASE(splitStringTwo)
{
    PRINT(WRAP("..."));

    std::string source = "";
    std::vector<std::string> result = {""};

    BOOST_CHECK(utils::splitString(source, " ") == result);
}

BOOST_AUTO_TEST_CASE(splitStringThree)
{
    PRINT(WRAP("..."));

    std::string source = " ";
    std::vector<std::string> result = {""};

    BOOST_CHECK(utils::splitString(source, "") == result);
}

BOOST_AUTO_TEST_CASE(splitStringFour)
{
    PRINT(WRAP("..."));

    std::string source = " ";
    std::vector<std::string> result = {""};

    BOOST_CHECK(utils::splitString(source, " ") == result);
}

BOOST_AUTO_TEST_CASE(splitStringFive)
{
    PRINT(WRAP("..."));

    std::string source = "";
    std::vector<std::string> result = {""};

    BOOST_CHECK(utils::splitString(source, "0") == result);
}

BOOST_AUTO_TEST_CASE(splitStringSix)
{
    PRINT(WRAP("..."));

    std::string source = ".";
    std::vector<std::string> result = {"."};

    BOOST_CHECK(utils::splitString(source, "") == result);
}

BOOST_AUTO_TEST_CASE(splitStringSeven)
{
    PRINT(WRAP("..."));

    std::string source = ".";
    std::vector<std::string> result = {"", ""};

    BOOST_CHECK(utils::splitString(source, ".") == result);
}

BOOST_AUTO_TEST_CASE(splitStringEight)
{
    PRINT(WRAP("..."));

    std::string source = "..";
    std::vector<std::string> result = {"", "", ""};

    BOOST_CHECK(utils::splitString(source, ".") == result);
}

BOOST_AUTO_TEST_CASE(splitStringNine)
{
    PRINT(WRAP("..."));

    std::string source = ".,.";
    std::vector<std::string> result = {"", ",", ""};

    BOOST_CHECK(utils::splitString(source, ".") == result);
}

BOOST_AUTO_TEST_CASE(splitStringTen)
{
    PRINT(WRAP("..."));

    std::string source = ".,.";
    std::vector<std::string> result = {"", "", "", ""};

    BOOST_CHECK(utils::splitString(source, ".,") == result);
}

BOOST_AUTO_TEST_CASE(splitStringEleven)
{
    PRINT(WRAP("..."));

    std::string source = " . , . ";
    std::vector<std::string> result = {"", "", "", ""};

    BOOST_CHECK(utils::splitString(source, ".,") == result);
}

BOOST_AUTO_TEST_CASE(splitStringTwelve)
{
    PRINT(WRAP("..."));

    std::string source = "a";
    std::vector<std::string> result = {"a"};

    BOOST_CHECK(utils::splitString(source, ".") == result);
}

BOOST_AUTO_TEST_CASE(splitStringThirteen)
{
    PRINT(WRAP("..."));

    std::string source = "1,2";
    std::vector<std::string> result = {"1,2"};

    BOOST_CHECK(utils::splitString(source, ".") == result);
}

BOOST_AUTO_TEST_CASE(splitStringFourteen)
{
    PRINT(WRAP("..."));

    std::string source = "1,2";
    std::vector<std::string> result = {"1", "2"};

    BOOST_CHECK(utils::splitString(source, ",") == result);
}

BOOST_AUTO_TEST_CASE(splitStringFifteen)
{
    PRINT(WRAP("..."));

    std::string source = "1,2";
    std::vector<std::string> result = {"1", "2"};

    BOOST_CHECK(utils::splitString(source, ".,") == result);
}

BOOST_AUTO_TEST_CASE(splitStringSixteen)
{
    PRINT(WRAP("..."));

    std::string source = "1.,2";
    std::vector<std::string> result = {"1", "", "2"};

    BOOST_CHECK(utils::splitString(source, ".,") == result);
}

BOOST_AUTO_TEST_CASE(splitStringSeventeen)
{
    PRINT(WRAP("..."));

    std::string source = "1,.2";
    std::vector<std::string> result = {"1", "", "2"};

    BOOST_CHECK(utils::splitString(source, ".,") == result);
}

BOOST_AUTO_TEST_CASE(splitStringEighteen)
{
    PRINT(WRAP("..."));

    std::string source = "1.,.2";
    std::vector<std::string> result = {"1", "", "", "2"};

    BOOST_CHECK(utils::splitString(source, ".,") == result);
}


BOOST_AUTO_TEST_CASE(data)
{
    PRINT(WRAP("..."));

    std::stringstream ss;
    time_t t = std::time(nullptr);
    auto tm = *localtime(&t);
    ss << std::put_time(&tm, "%d.%m.%Y");

    BOOST_CHECK_EQUAL(utils::date(), ss.str());
}


BOOST_AUTO_TEST_CASE(findAgeOne)
{
    PRINT(WRAP("..."));

    std::vector<std::string> data = {"28", "02", "2019"};
    std::vector<std::string> dateOfBirth = {"29", "02", "2020"};

    BOOST_CHECK(utils::findAge(data, dateOfBirth)[0] < 0);
}

BOOST_AUTO_TEST_CASE(findAgeTwo)
{
    PRINT(WRAP("..."));

    std::vector<std::string> data = {"28", "02", "2019"};
    std::vector<std::string> dateOfBirth = {"28", "02", "2020"};

    BOOST_CHECK(utils::findAge(data, dateOfBirth)[0] < 0);
}

BOOST_AUTO_TEST_CASE(findAgeThree)
{
    PRINT(WRAP("..."));

    std::vector<std::string> data = {"29", "02", "2016"};
    std::vector<std::string> dateOfBirth = {"28", "02", "2017"};

    BOOST_CHECK(utils::findAge(data, dateOfBirth)[0] < 0);
}

BOOST_AUTO_TEST_CASE(findAgeFour)
{
    PRINT(WRAP("..."));

    std::vector<std::string> data = {"28", "02", "2016"};
    std::vector<std::string> dateOfBirth = {"28", "02", "2017"};

    BOOST_CHECK(utils::findAge(data, dateOfBirth)[0] < 0);
}

BOOST_AUTO_TEST_CASE(findAgeFive)
{
    PRINT(WRAP("..."));

    std::vector<std::string> data = {"29", "02", "2020"};
    std::vector<std::string> dateOfBirth = {"28", "02", "2019"};
    std::vector<int> age = {1, 0, 1};

    BOOST_CHECK(utils::findAge(data, dateOfBirth) == age);
}

BOOST_AUTO_TEST_CASE(findAgeSix)
{
    PRINT(WRAP("..."));

    std::vector<std::string> data = {"28", "02", "2020"};
    std::vector<std::string> dateOfBirth = {"28", "02", "2019"};
    std::vector<int> age = {1, 0, 0};

    BOOST_CHECK(utils::findAge(data, dateOfBirth) == age);
}

BOOST_AUTO_TEST_CASE(findAgeSeven)
{
    PRINT(WRAP("..."));

    std::vector<std::string> data = {"28", "02", "2017"};
    std::vector<std::string> dateOfBirth = {"29", "02", "2016"};
    std::vector<int> age = {0, 11, 28};

    BOOST_CHECK(utils::findAge(data, dateOfBirth) == age);
}

BOOST_AUTO_TEST_CASE(findAgeEight)
{
    PRINT(WRAP("..."));

    std::vector<std::string> data = {"28", "02", "2017"};
    std::vector<std::string> dateOfBirth = {"28", "02", "2016"};
    std::vector<int> age = {1, 0, 0};

    BOOST_CHECK(utils::findAge(data, dateOfBirth) == age);
}


BOOST_AUTO_TEST_CASE(toUpperAndToLowerOne)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("test"), "Test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerTwo)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("Test"), "Test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerThree)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("tesT"), "Test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerFour)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("TesT"), "Test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerFive)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("TEST"), "Test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerSix)
{
    PRINT(WRAP("..."));

    std::string source = "`~!@#$%^&*()_+{}:<>?/|\"№;-=,'.[]\\1234567890"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string result = "`~!@#$%^&*()_+{}:<>?/|\"№;-=,'.[]\\1234567890"
                         "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower(source), result);
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerSeven)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("test", 0), "test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerEight)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("Test", 0), "test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerNine)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("tesT", 0), "test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerTen)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("TesT", 0), "test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerEleven)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("TEST", 0), "test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerTwelve)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("test", 1), "Test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerThirteen)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("Test", 1), "Test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerFourteen)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("tesT", 1), "Test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerFifteen)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("TesT", 1), "Test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerSixteen)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("TEST", 1), "Test");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerSeventeen)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("test", 2), "TEst");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerEighteen)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("Test", 2), "TEst");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerNineteen)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("tesT", 2), "TEst");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerTwenty)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("TesT", 2), "TEst");
}

BOOST_AUTO_TEST_CASE(toUpperAndToLowerTwentyOne)
{
    PRINT(WRAP("..."));

    BOOST_CHECK_EQUAL(utils::toUpperAndToLower("TEST", 2), "TEst");
}


BOOST_AUTO_TEST_CASE(toupperandtolowerOne)
{
    PRINT(WRAP("..."));

    std::string source = "test";
    std::string result = "Test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerTwo)
{
    PRINT(WRAP("..."));

    std::string source = "Test";
    std::string result = "Test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerThree)
{
    PRINT(WRAP("..."));

    std::string source = "tesT";
    std::string result = "Test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerFour)
{
    PRINT(WRAP("..."));

    std::string source = "TesT";
    std::string result = "Test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerFive)
{
    PRINT(WRAP("..."));

    std::string source = "TEST";
    std::string result = "Test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerSix)
{
    PRINT(WRAP("..."));

    std::string source = "`~!@#$%^&*()_+{}:<>?/|\"№;-=,'.[]\\1234567890"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string result = "`~!@#$%^&*()_+{}:<>?/|\"№;-=,'.[]\\1234567890"
                         "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerSeven)
{
    PRINT(WRAP("..."));

    std::string source = "test";
    std::string result = "test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 0));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerEight)
{
    PRINT(WRAP("..."));

    std::string source = "Test";
    std::string result = "test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 0));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerNine)
{
    PRINT(WRAP("..."));

    std::string source = "tesT";
    std::string result = "test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 0));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerTen)
{
    PRINT(WRAP("..."));

    std::string source = "TesT";
    std::string result = "test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 0));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerEleven)
{
    PRINT(WRAP("..."));

    std::string source = "TEST";
    std::string result = "test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 0));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerTwelve)
{
    PRINT(WRAP("..."));

    std::string source = "test";
    std::string result = "Test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 1));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerThirteen)
{
    PRINT(WRAP("..."));

    std::string source = "Test";
    std::string result = "Test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 1));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerFourteen)
{
    PRINT(WRAP("..."));

    std::string source = "tesT";
    std::string result = "Test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 1));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerFifteen)
{
    PRINT(WRAP("..."));

    std::string source = "TesT";
    std::string result = "Test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 1));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerSixteen)
{
    PRINT(WRAP("..."));

    std::string source = "TEST";
    std::string result = "Test";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 1));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerSeventeen)
{
    PRINT(WRAP("..."));

    std::string source = "test";
    std::string result = "TEst";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 2));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerEighteen)
{
    PRINT(WRAP("..."));

    std::string source = "Test";
    std::string result = "TEst";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 2));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerNineteen)
{
    PRINT(WRAP("..."));

    std::string source = "tesT";
    std::string result = "TEst";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 2));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerTwenty)
{
    PRINT(WRAP("..."));

    std::string source = "TesT";
    std::string result = "TEst";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 2));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_CASE(toupperandtolowerTwentyOne)
{
    PRINT(WRAP("..."));

    std::string source = "TEST";
    std::string result = "TEst";

    BOOST_CHECK_NO_THROW(utils::toupperandtolower(source, 2));
    BOOST_CHECK(source == result);
}

BOOST_AUTO_TEST_SUITE_END()
