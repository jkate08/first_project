#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace std;

// Класс для работы с курсами валют
class CurrencyConverter {
private:
    map<string, double> exchangeRates;
    string baseCurrency;

public:
    // Загрузка курсов валют из файла
    bool loadExchangeRates(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Ошибка: Не удалось открыть файл с курсами валют." << endl;
            return false;
        }

        string line;
        // Первая строка - базовая валюта
        if (getline(file, line)) {
            baseCurrency = line;
        } else {
            cerr << "Ошибка: Файл курсов валют пуст или имеет неверный формат." << endl;
            return false;
        }

        // Последующие строки - пары валюта:курс
        while (getline(file, line)) {
            size_t delimiterPos = line.find(':');
            if (delimiterPos == string::npos) {
                continue; // Пропускаем строки с неверным форматом
            }

            string currency = line.substr(0, delimiterPos);
            double rate = stod(line.substr(delimiterPos + 1));

            exchangeRates[currency] = rate;
        }

        file.close();
        return true;
    }

    // Конвертация суммы из одной валюты в другую
    double convert(double amount, const string& fromCurrency, const string& toCurrency) {
        if (fromCurrency == toCurrency) {
            return amount;
        }

        // Если одна из валют - базовая
        if (fromCurrency == baseCurrency) {
            if (exchangeRates.find(toCurrency) != exchangeRates.end()) {
                return amount * exchangeRates[toCurrency];
            }
        } else if (toCurrency == baseCurrency) {
            if (exchangeRates.find(fromCurrency) != exchangeRates.end()) {
                return amount / exchangeRates[fromCurrency];
            }
        } else {
            // Конвертация между двумя небазовыми валютами
            if (exchangeRates.find(fromCurrency) != exchangeRates.end() &&
                exchangeRates.find(toCurrency) != exchangeRates.end()) {
                return amount * exchangeRates[toCurrency] / exchangeRates[fromCurrency];
            }
        }

        cerr << "Ошибка: Неизвестная валюта или отсутствует курс обмена." << endl;
        return -1.0;
    }

    // Получение списка доступных валют
    vector<string> getAvailableCurrencies() const {
        vector<string> currencies;
        currencies.push_back(baseCurrency);
        for (const auto& pair : exchangeRates) {
            currencies.push_back(pair.first);
        }
        return currencies;
    }

    // Проверка наличия валюты
    bool hasCurrency(const string& currency) const {
        return currency == baseCurrency || exchangeRates.find(currency) != exchangeRates.end();
    }
};

// Функция для вывода помощи
void printHelp() {
    cout << "Доступные команды:\n"
         << "help - показать это сообщение\n"
         << "list - показать список доступных валют\n"
         << "convert - конвертировать сумму из одной валюты в другую\n"
         << "exit - выйти из программы\n";
}

// Функция для вывода списка валют
void printCurrencyList(const CurrencyConverter& converter) {
    vector<string> currencies = converter.getAvailableCurrencies();
    cout << "Доступные валюты:\n";
    for (const string& currency : currencies) {
        cout << "- " << currency << "\n";
    }
}

// Функция для обработки команды конвертации
void handleConvertCommand(CurrencyConverter& converter) {
    string fromCurrency, toCurrency;
    double amount;

    cout << "Введите исходную валюту: ";
    cin >> fromCurrency;
    transform(fromCurrency.begin(), fromCurrency.end(), fromCurrency.begin(), ::toupper);

    if (!converter.hasCurrency(fromCurrency)) {
        cout << "Ошибка: Валюта '" << fromCurrency << "' не поддерживается.\n";
        return;
    }
cout << "Введите целевую валюту: ";
    cin >> toCurrency;
    transform(toCurrency.begin(), toCurrency.end(), toCurrency.begin(), ::toupper);

    if (!converter.hasCurrency(toCurrency)) {
        cout << "Ошибка: Валюта '" << toCurrency << "' не поддерживается.\n";
        return;
    }

    cout << "Введите сумму для конвертации: ";
    cin >> amount;

    if (amount <= 0) {
        cout << "Ошибка: Сумма должна быть положительным числом.\n";
        return;
    }

    double result = converter.convert(amount, fromCurrency, toCurrency);
    if (result >= 0) {
        cout << fixed << setprecision(2);
        cout << amount << " " << fromCurrency << " = " << result << " " << toCurrency << "\n";
    }
}

// Основная функция
int main() {
    CurrencyConverter converter;

    // Загрузка курсов валют (файл должен быть в том же каталоге)
    if (!converter.loadExchangeRates("/home/jkate/CurrentyBot/exchange_rates.txt")) {
        return 1;
    }

    cout << "Добро пожаловать в чат-бот конвертации валют!\n";
    cout << "Введите 'help' для списка команд.\n";

    string command;
    while (true) {
        cout << "> ";
        cin >> command;
        transform(command.begin(), command.end(), command.begin(), ::tolower);

        if (command == "exit") {
            break;
        } else if (command == "help") {
            printHelp();
        } else if (command == "list") {
            printCurrencyList(converter);
        } else if (command == "convert") {
            handleConvertCommand(converter);
        } else {
            cout << "Неизвестная команда. Введите 'help' для списка команд.\n";
        }
    }

    cout << "До свидания!\n";
    return 0;
}
