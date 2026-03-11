#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>       // Untuk Sistem Timer
#include <unordered_map> // Untuk menghitung frekuensi
#include <algorithm>    // Untuk sorting

using namespace std;
using namespace std::chrono;

// ==========================================
// 1. SETUP ADT (Abstract Data Type)
// ==========================================
struct Transaksi {
    string id_transaksi;
    string id_pelanggan;
    string id_produk;
    string nama_produk;
    string kategori_produk;
    int jumlah_pembelian;
    string tanggal_transaksi;
};

struct ProdukCount {
    string nama_produk;
    int frekuensi;
};

bool compareProduk(ProdukCount a, ProdukCount b) {
    return a.frekuensi > b.frekuensi;
}

// ==========================================
// 2. FILE I/O (Membaca CSV ke Vector)
// ==========================================

void loadData(string filename, vector<Transaksi>& dataset, int limit = -1) {
    ifstream file(filename);
    string line, word;

    if (!file.is_open()) {
        cout << "Error: File " << filename << " tidak ditemukan!" << endl;
        return;
    }

    getline(file, line);

    int count = 0;
    while (getline(file, line)) {

        if (limit != -1 && count >= limit) break;

        stringstream s(line);
        Transaksi t;
        string temp_jumlah;

        getline(s, t.id_transaksi, ',');
        getline(s, t.id_pelanggan, ',');
        getline(s, t.id_produk, ',');
        getline(s, t.nama_produk, ',');
        getline(s, t.kategori_produk, ',');
        getline(s, temp_jumlah, ',');
        getline(s, t.tanggal_transaksi, ',');

        try {
            t.jumlah_pembelian = stoi(temp_jumlah);
        } catch (...) {
            t.jumlah_pembelian = 0;
        }

        dataset.push_back(t);
        count++;
    }
    file.close();
    cout << "Berhasil memuat " << dataset.size() << " baris data." << endl;
}

// ==========================================
// 3. LOGIKA REKOMENDASI (Top-N Terlaris)
// ==========================================
void generateTopN(const vector<Transaksi>& dataset, int n) {
    unordered_map<string, int> frekuensi_produk;
    for (const auto& t : dataset) {
        frekuensi_produk[t.nama_produk] += t.jumlah_pembelian;
    }

    vector<ProdukCount> list_frekuensi;
    for (const auto& pair : frekuensi_produk) {
        list_frekuensi.push_back({pair.first, pair.second});
    }

    sort(list_frekuensi.begin(), list_frekuensi.end(), compareProduk);

    cout << "\n--- REKOMENDASI TOP " << n << " PRODUK TERLARIS ---\n";
    int batas = min(n, (int)list_frekuensi.size());
    for (int i = 0; i < batas; i++) {
        cout << i + 1 << ". " << list_frekuensi[i].nama_produk 
             << " (Terjual: " << list_frekuensi[i].frekuensi << " item)\n";
    }
}

// 4. MAIN PROGRAM & SISTEM TIMER
int main() {
    vector<Transaksi> riwayat_transaksi;
    int jumlah_data_uji = 5378; 
    
    cout << "=== MEMULAI PROGRAM ===" << endl;
    
    auto start_load = high_resolution_clock::now();
    loadData("dataset_rekomendasi_final.csv", riwayat_transaksi, jumlah_data_uji);
    auto stop_load = high_resolution_clock::now();
    auto duration_load = duration_cast<milliseconds>(stop_load - start_load);
    cout << "Waktu Load Data: " << duration_load.count() << " ms\n";

    auto start_rekomendasi = high_resolution_clock::now();
    generateTopN(riwayat_transaksi, 5);
    auto stop_rekomendasi = high_resolution_clock::now();
    auto duration_rekomendasi = duration_cast<milliseconds>(stop_rekomendasi - start_rekomendasi);
    
    cout << "\nWaktu Eksekusi Rekomendasi: " << duration_rekomendasi.count() << " ms\n";
    cout << "=======================" << endl;

    return 0;
}