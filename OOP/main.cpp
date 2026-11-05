
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <algorithm>

// ==========================================
// CONFIGURATION
// ==========================================

#define INPUT_FILE "tests/data.csv"

#define STR(x) #x
#define XSTR(x) STR(x)
#define COL(c) "\033[" XSTR(c) "m"

#define RED 31
#define GREEN 32
#define BLUE 34
#define CYAN 36
#define MAGENTA 35
#define YELLOW 33
#define RESET 0

struct SensorInfo
{
    std::string id;
    std::string unit;
};

static const std::array<SensorInfo, 10> SENSOR_LIST =
    {{{"CO2", "ppm"},
      {"CURR", "A"},
      {"HUMID", "%"},
      {"LIGHT", "lux"},
      {"PRESS", "hPa"},
      {"SOIL", "%"},
      {"SOUND", "dB"},
      {"TEMP", "°C"},
      {"VIB", "mm/s"},
      {"VOLT", "V"}}};

class Record
{
public:
    uint64_t ms = 0;
    double data = 0.0;

    std::string label;
    std::string sensorID;

    Record() = default;

    Record(
        uint64_t timestamp,
        double value,
        const std::string &lbl,
        const std::string &sensor)
        : ms(timestamp),
          data(value),
          label(lbl),
          sensorID(sensor)
    {
    }
};

class SensorStats
{
private:
    SensorInfo info;

    double maxValue = 0.0;
    double minValue = 0.0;
    double sum = 0.0;

    uint32_t count = 0;

public:
    SensorStats() = default;

    explicit SensorStats(const SensorInfo &sensor)
        : info(sensor)
    {
    }

    void addSample(double value)
    {
        if (count == 0)
        {
            maxValue = value;
            minValue = value;
        }
        else
        {
            maxValue = std::max(maxValue, value);
            minValue = std::min(minValue, value);
        }

        sum += value;
        count++;
    }

    double getMax() const
    {
        return maxValue;
    }

    double getMin() const
    {
        return minValue;
    }

    double getAverage() const
    {
        return count ? sum / count : 0.0;
    }

    uint32_t getCount() const
    {
        return count;
    }

    const std::string &getID() const
    {
        return info.id;
    }

    const std::string &getUnit() const
    {
        return info.unit;
    }
};

class CSVReader
{
private:
    Record parseRecord(const std::string &line) const
    {
        std::stringstream stream(line);

        std::string timestamp;
        std::string value;
        std::string label;
        std::string sensor;

        if (!std::getline(stream, timestamp, ',') ||
            !std::getline(stream, value, ',') ||
            !std::getline(stream, label, ',') ||
            !std::getline(stream, sensor, ','))
        {
            throw std::runtime_error("Invalid CSV record");
        }

        size_t timestampPos = 0;
        size_t valuePos = 0;

        uint64_t ms = std::stoull(timestamp, &timestampPos);
        double data = std::stod(value, &valuePos);

        if (timestampPos != timestamp.size() ||
            valuePos != value.size())
        {
            throw std::runtime_error("Invalid numeric field");
        }

        return Record(ms, data, label, sensor);
    }

public:
    std::vector<Record> readFile(const std::string &filename) const
    {
        std::ifstream file(filename);

        if (!file.is_open())
        {
            throw std::runtime_error(
                "Could not open file: " + filename);
        }

        std::cout << COL(GREEN)
                  << "\tOpened file '" << filename << "'\n";

        std::vector<Record> records;

        std::string line;

        // Skip CSV header
        std::getline(file, line);

        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            records.push_back(parseRecord(line));
        }

        file.close();

        std::cout << "\n\tClosed file '"
                  << filename << "'\n"
                  << COL(RESET);

        return records;
    }
};

class SensorDashboard
{
private:
    std::array<SensorStats, 10> sensors;

    size_t totalRecords = 0;

    int sensorToIndex(const std::string &id) const
    {
        for (size_t i = 0; i < SENSOR_LIST.size(); i++)
        {
            if (SENSOR_LIST[i].id == id)
                return static_cast<int>(i);
        }

        return -1;
    }

public:
    SensorDashboard()
    {
        for (size_t i = 0; i < sensors.size(); i++)
        {
            sensors[i] = SensorStats(SENSOR_LIST[i]);
        }
    }

    void process(const std::vector<Record> &records)
    {
        totalRecords = records.size();

        for (const Record &record : records)
        {
            int idx = sensorToIndex(record.sensorID);

            if (idx == -1)
            {
                std::cerr << COL(RED)
                          << "Unknown sensor ID: "
                          << record.sensorID
                          << COL(RESET) << '\n';

                continue;
            }

            sensors[idx].addSample(record.data);
        }
    }

    void display() const
    {
        std::cout << COL(RESET)
                  << "===========================================\n";

        std::cout << COL(MAGENTA)
                  << "           SENSOR DATA AGGREGATE          \n";

        std::cout << COL(RESET)
                  << "===========================================\n\n";

        // Sensor statistics

        for (const SensorStats &sensor : sensors)
        {
            if (sensor.getCount() == 0)
                continue;

            std::cout << COL(MAGENTA)
                      << "Sensor : " << std::left
                      << std::setw(24) << sensor.getID()
                      << "Unit : " << sensor.getUnit()
                      << "\n\n";

            std::cout << std::fixed << std::setprecision(3);

            std::cout << COL(RED)
                      << "MAX :______________________________"
                      << sensor.getMax() << '\n';

            std::cout << COL(GREEN)
                      << "AVG :_________________"
                      << sensor.getAverage() << '\n';

            std::cout << COL(BLUE)
                      << "MIN :_______"
                      << sensor.getMin() << "\n\n";

            std::cout << COL(RESET)
                      << "===========================================\n\n";
        }

        // Sample counts

        std::cout << COL(CYAN)
                  << "SAMPLES EXAMINED\n"
                  << COL(RESET);

        for (const SensorStats &sensor : sensors)
        {
            if (sensor.getCount() == 0)
                continue;

            std::cout << COL(YELLOW)
                      << std::setw(8) << sensor.getID()
                      << "  :   " << COL(RESET)
                      << "[=========100%=======>] "
                      << COL(GREEN)
                      << sensor.getCount()
                      << COL(RESET)
                      << " samples\n";
        }

        std::cout << "\n\n============================================\n\n";

        std::cout << "Total records processed: "
                  << totalRecords << '\n';
    }
};

int main()
{
    try
    {
        std::cout << "\n\t"
                  << COL(MAGENTA)
                  << "Hello from OOP example\n"
                  << COL(RESET);

        // Create objects

        CSVReader reader;
        SensorDashboard dashboard;

        // Read CSV

        std::vector<Record> records =
            reader.readFile(INPUT_FILE);

        // Aggregate sensor data

        dashboard.process(records);

        // Display results

        dashboard.display();

        std::cout << "\n"
                  << COL(RESET);
    }
    catch (const std::exception &e)
    {
        std::cerr << COL(RED)
                  << "ERROR: " << e.what()
                  << COL(RESET) << '\n';

        return 1;
    }

    return 0;
}
