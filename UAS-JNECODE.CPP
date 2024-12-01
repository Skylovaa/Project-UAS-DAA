#include <bits/stdc++.h>
using namespace std;

// Struktur untuk menyimpan informasi penerbangan
struct DeliveryState {
    string Origin;
    string Transit;
    string Airport;
    string Airlines;
    string Destination;
    string TrafficCondition;
    string WeatherCondition;
    bool VehicleAvailable;
    bool PersonelAvailable;
    int EstimatedTime_menit;
    string EstimatedTime_spesifik;
    string AdditionalNotes;
    int RunawayOccupancyTime;

    // Fungsi untuk menghitung total waktu perjalanan
    int totalScore(const map<string, int>& historicalData) const {
        int totalScore = EstimatedTime_menit; // Mulai dengan waktu estimasi perjalanan

        // Jika kendaraan tidak tersedia, kita tidak menghitung origin ini
        if (!VehicleAvailable) {
            return INT_MAX; // Mengembalikan nilai maksimum untuk menandakan bahwa ini bukan pilihan yang baik
        }

        // Menambahkan waktu jika personel tidak tersedia
        if (!PersonelAvailable) {
            totalScore += 240; // Delay 4 jam jika tidak ada personel
        }

        // Menambahkan runway occupancy time
        totalScore += RunawayOccupancyTime;

        // Menambahkan waktu berdasarkan kondisi lalu lintas
        if (TrafficCondition == "Moderate") {
            totalScore += 100;
        } else if (TrafficCondition == "Heavy") {
            totalScore += 200;
        }

        // Menambahkan waktu berdasarkan kondisi cuaca
        if (WeatherCondition == "Rain") {
            totalScore += 150;
        } else if (WeatherCondition == "Fog") {
            totalScore += 250;
        }

        // Adaptasi berdasarkan data historis ADP 
        string key = Origin + "-" + Transit + "-" + Destination;
        if (historicalData.find(key) != historicalData.end()) {
            // Rata-rata dengan data historis untuk memperbaiki estimasi waktu
            totalScore = (totalScore + historicalData.at(key)) / 2; // Mengadaptasi estimasi waktu
        }

        return totalScore;
    }
};

// Fungsi untuk menghapus spasi ekstra
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// Fungsi untuk membaca data dari file CSV
vector<pair<DeliveryState, int>> readCSV(const string& filename) {
    vector<pair<DeliveryState, int>> flights;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Tidak dapat membuka file: " << filename << endl;
        return flights;
    }

    string line;
    int lineNumber = 0;
    while (getline(file, line)) {
        lineNumber++;
        stringstream ss(line);
        string value;
        DeliveryState flight;

        getline(ss, flight.Origin, ',');
        getline(ss, flight.Transit, ',');
        getline(ss, flight.Airport, ',');
        getline(ss, flight.Airlines, ',');
        getline(ss, flight.Destination, ',');
        getline(ss, flight.TrafficCondition, ',');
        getline(ss, flight.WeatherCondition, ',');
        getline(ss, value, ','); 
        flight.VehicleAvailable = (value == "YESS");
        getline(ss, value, ','); 
        flight.PersonelAvailable = (value == "YESS");

        getline(ss, value, ','); 
        try {
            flight.EstimatedTime_menit = stoi(trim(value));
        } catch (const std::invalid_argument& e) {
            cerr << "Invalid EstimatedTime_menit value: " << value << endl;
            flight.EstimatedTime_menit = 0;
        }

        getline(ss, flight.EstimatedTime_spesifik, ',');
        getline(ss, value); 
        try {
            flight.RunawayOccupancyTime = stoi(trim(value));
        } catch (const std:: invalid_argument& e) {
            cerr << "Invalid RunawayOccupancyTime value: " << value << endl;
            flight.RunawayOccupancyTime = 0;
        }

        flight.Origin = trim(flight.Origin);
        flight.Transit = trim(flight.Transit);
        flight.Airport = trim(flight.Airport);
        flight.Airlines = trim(flight.Airlines);
        flight.Destination = trim(flight.Destination);
        flight.TrafficCondition = trim(flight.TrafficCondition);
        flight.WeatherCondition = trim(flight.WeatherCondition);
        flight.EstimatedTime_spesifik = trim(flight.EstimatedTime_spesifik);
        flights.emplace_back(flight, lineNumber);
    }

    file.close();
    return flights;
}

void displayFlightInfo(const DeliveryState& flight, int lineNumber, const string& datasetName, const map<string, int>& historicalData) {
    cout << "#" << lineNumber << " from " << datasetName << endl;
    cout << "Origin                  : " << flight.Origin << endl;
    cout << "Transit                 : " << flight.Transit << endl;
    cout << "Airport                 : " << flight.Airport << endl;
    cout << "Airlines                : " << flight.Airlines << endl;
    cout << "Destination             : " << flight.Destination << endl;
    cout << "Traffic Condition        : " << flight.TrafficCondition << endl;
    cout << "Weather Condition        : " << flight.WeatherCondition << endl;
    cout << "Vehicle Available        : " << (flight.VehicleAvailable ? "Yes" : "No") << endl;

    int totalEstimatedTime = flight.totalScore(historicalData);
    int hours = totalEstimatedTime / 60;
    int minutes = totalEstimatedTime % 60;
    cout << "Estimated Time (minutes) : " << flight.EstimatedTime_menit << " minutes" << endl;
    cout << "Estimated Time (specific) : " << hours << " jam " << minutes << " menit" << endl;
    cout << "Additional Notes         : " << flight.AdditionalNotes << endl;
    cout << endl;
}

int main() {
    string filename1 = "dataset_bts.csv"; 
    string filename2 = "dataset_exo.csv"; 
    string datasetName1 = "dataset_bts"; 
    string datasetName2 = "dataset_exo"; 

    vector<pair<DeliveryState, int>> flights1 = readCSV(filename1);
    vector<pair<DeliveryState, int>> flights2 = readCSV(filename2);

    map<string, int> historicalData; // Menyimpan data historis untuk adaptasi

    string destinationInput;
    cout << "Masukkan nama kota tujuan: ";
    getline(cin, destinationInput);
    trim(destinationInput);

    bool found = false;
    int totalFlightsFound = 0; 
    map<string, pair<DeliveryState, int>> bestFlights; // Mengubah key menjadi string
    vector<pair<DeliveryState, int>> bestJakartaFlights; 
    map<string, int> totalEstimatedTimeMap; 

    // ===========================
    // Proses penerbangan dari dataset 1
    // ===========================
    for (const auto& flight : flights1) {
        if (trim(flight.first.Destination) == destinationInput) {
            displayFlightInfo(flight.first, flight.second, datasetName1, historicalData);
            found = true;
            totalFlightsFound++;

            if (flight.first.VehicleAvailable) {
                string key = flight.first.Origin + "-" + flight.first.Transit; // Menggunakan string sebagai key
                // ===========================
                // Menyimpan estimasi waktu ke historicalData
                historicalData[key] = flight.first.EstimatedTime_menit; 
                if (bestFlights.find(key) == bestFlights.end() || flight.first.totalScore(historicalData) < bestFlights[key].first.totalScore(historicalData)) {
                    bestFlights[key] = flight; // Simpan penerbangan terbaik
                }
            }
        }
    }

    // ===========================
    // Proses penerbangan dari dataset 2
    // ===========================
    for (const auto& flight : flights2) {
        if (trim(flight.first.Destination) == destinationInput) {
            displayFlightInfo(flight.first, flight.second, datasetName2, historicalData);
            found = true;
            totalFlightsFound++;

            if (flight.first.VehicleAvailable) {
                string key = flight.first.Origin + "-" + flight.first.Transit; // Menggunakan string sebagai key
                // ===========================
                // Menyimpan estimasi waktu ke historicalData
                historicalData[key] = flight.first.EstimatedTime_menit; 
                if (bestFlights.find(key) == bestFlights.end() || flight.first.totalScore(historicalData) < bestFlights[key].first.totalScore(historicalData)) {
                    bestFlights[key] = flight; // Simpan penerbangan terbaik
                }
            }
        }
    }

    if (!found) {
        cout << "Tidak ada penerbangan yang ditemukan untuk kota tujuan: " << destinationInput << endl;
    } else {
        cout << "JUMBLAH PENERBANGAN YANG DI TEMUKAN  : " << totalFlightsFound << endl;

        // Menampilkan penerbangan terbaik untuk setiap Origin
        cout << "=================================================\n";
        cout << "PENERBANGAN TERBAIK UNTUK SETIAP ORIGIN " << endl;
        cout << "=================================================\n";

        string transitFromJakarta;
        map<string, string> bestEstimatedTimeMap; 
        map<string, int> totalTimeByOrigin;  // Untuk menyimpan total waktu berdasarkan asal
        map<string, string> bestTransitForDestination;  // Untuk menyimpan transit terbaik berdasarkan waktu tercepat

        for (const auto& entry : bestFlights) {
            int totalEstimatedTime = entry.second.first.totalScore(historicalData); // Waktu total menggunakan totalScore()
            string origin = entry.first.substr(0, entry.first.find('-')); // Mendapatkan origin dari key
            string destination = entry.first.substr(entry.first.find('-') + 1); // Mendapatkan transit dari key

            // Jika asal adalah Jakarta(CGK), tambahkan waktu ke tujuan dan cari transit terbaik
            if (origin == "Jakarta(CGK)") {
                transitFromJakarta = destination; 
                displayFlightInfo(entry.second.first, entry.second.second, "Best Flight from " + origin + " to " + destination, historicalData);
                bestJakartaFlights.push_back(entry.second);
                bestEstimatedTimeMap[transitFromJakarta] = entry.second.first.EstimatedTime_spesifik;

                // Men yimpan waktu total penerbangan untuk tujuan
                totalTimeByOrigin[destination] += totalEstimatedTime;
            } else {
                displayFlightInfo(entry.second.first, entry.second.second, "Best Flight from " + origin + " to " + destination, historicalData);

                // Menyimpan waktu total penerbangan untuk tujuan
                totalTimeByOrigin[destination] += totalEstimatedTime;
            }
        }

        // Menampilkan hasil penjumlahan waktu berdasarkan asal
        cout << "\nHasil Penjumlahan Waktu Penerbangan berdasarkan Transit :" << endl;

        // Untuk mencari transit terbaik berdasarkan waktu tercepat
        int minTime = INT_MAX;
        string bestTransitDestination;

        // Menghitung waktu transit terbaik untuk setiap tujuan
        for (const auto& entry : totalTimeByOrigin) {
            int totalEstimatedTime = entry.second;
            int hours = totalEstimatedTime / 60;
            int minutes = totalEstimatedTime % 60;

            cout << "Transit : " << entry.first << " : " << hours << " jam " << minutes << " menit" << endl;

            // Mencari transit dengan waktu tercepat
            if (totalEstimatedTime < minTime) {
                minTime = totalEstimatedTime;
                bestTransitDestination = entry.first;
            }
        }

        // Menampilkan Transit terbaik berdasarkan waktu tercepat
        if (!bestTransitDestination.empty()) {
            int bestHours = minTime / 60;
            int bestMinutes = minTime % 60;
            cout << "\nTransit terbaik " << bestTransitDestination << " : " << bestHours << " jam " << bestMinutes << " menit" << endl;
        }
    }

    return 0;
}
