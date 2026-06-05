#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <algorithm>
#include <limits>

using namespace std;
using namespace std::chrono;

// ==========================================
// BAGIAN 1 — ADT
// ==========================================

struct Transaksi {
    string id_transaksi;
    string id_pelanggan;
    string id_produk;
    string nama_produk;
    string kategori_produk;
    int    jumlah_pembelian;
    string tanggal_transaksi;
};

struct NodeBST {
    string            key;
    vector<Transaksi> transaksi_list;
    NodeBST*          left  = nullptr;
    NodeBST*          right = nullptr;

    NodeBST(const string& k, const Transaksi& t) : key(k) {
        transaksi_list.push_back(t);
    }
};

struct ProdukCount {
    string nama_produk;
    int    frekuensi;
};

bool compareProduk(const ProdukCount& a, const ProdukCount& b) {
    return a.frekuensi > b.frekuensi;
}

// ==========================================
// BAGIAN 2 — FILE I/O
// ==========================================

void loadData(const string& filename, vector<Transaksi>& dataset, int limit = -1) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: File '" << filename << "' tidak ditemukan!\n";
        return;
    }

    string line;
    getline(file, line); // lewati header

    int count = 0;
    while (getline(file, line)) {
        if (limit != -1 && count >= limit) break;

        stringstream s(line);
        Transaksi    t;
        string       temp_jumlah;

        getline(s, t.id_transaksi,      ',');
        getline(s, t.id_pelanggan,      ',');
        getline(s, t.id_produk,         ',');
        getline(s, t.nama_produk,       ',');
        getline(s, t.kategori_produk,   ',');
        getline(s, temp_jumlah,         ',');
        getline(s, t.tanggal_transaksi, ',');

        if (!t.tanggal_transaksi.empty() && t.tanggal_transaksi.back() == '\r')
            t.tanggal_transaksi.pop_back();

        try   { t.jumlah_pembelian = stoi(temp_jumlah); }
        catch (...) { t.jumlah_pembelian = 0; }

        dataset.push_back(t);
        count++;
    }
    file.close();
    cout << "Berhasil memuat " << dataset.size() << " baris data.\n";
}

// ==========================================
// BAGIAN 3 — OPERASI BST
// ==========================================

NodeBST* insertBST(NodeBST* root, const Transaksi& t) {
    if (root == nullptr)
        return new NodeBST(t.id_pelanggan, t);

    if (t.id_pelanggan < root->key)
        root->left = insertBST(root->left, t);
    else if (t.id_pelanggan > root->key)
        root->right = insertBST(root->right, t);
    else
        root->transaksi_list.push_back(t);

    return root;
}

NodeBST* buildBST(const vector<Transaksi>& dataset) {
    NodeBST* root = nullptr;
    for (const auto& t : dataset)
        root = insertBST(root, t);
    return root;
}

NodeBST* searchBST(NodeBST* root, const string& target) {
    if (root == nullptr) return nullptr;
    if (target == root->key) return root;
    return (target < root->key)
        ? searchBST(root->left,  target)
        : searchBST(root->right, target);
}

int countNodesBST(NodeBST* root) {
    if (root == nullptr) return 0;
    return 1 + countNodesBST(root->left) + countNodesBST(root->right);
}

// ==========================================
// BAGIAN 4 — OPERASI MAX-HEAP
// ==========================================

void heapify(vector<ProdukCount>& heap, int n, int i) {
    int largest = i;
    int left    = 2 * i + 1;
    int right   = 2 * i + 2;

    if (left  < n && heap[left].frekuensi  > heap[largest].frekuensi) largest = left;
    if (right < n && heap[right].frekuensi > heap[largest].frekuensi) largest = right;

    if (largest != i) {
        swap(heap[i], heap[largest]);
        heapify(heap, n, largest);
    }
}

void buildMaxHeap(vector<ProdukCount>& heap) {
    int n = heap.size();
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(heap, n, i);
}

ProdukCount extractMax(vector<ProdukCount>& heap) {
    ProdukCount top = heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    heapify(heap, heap.size(), 0);
    return top;
}

// ==========================================
// BAGIAN 5 — FITUR-FITUR UTAMA
// ==========================================

// Rekomendasi Top-N menggunakan Max-Heap
void generateTopN_Heap(const vector<Transaksi>& dataset, int n) {
    unordered_map<string, int> freq;
    for (const auto& t : dataset)
        freq[t.nama_produk] += t.jumlah_pembelian;

    vector<ProdukCount> heap;
    for (const auto& p : freq)
        heap.push_back({p.first, p.second});

    buildMaxHeap(heap);

    cout << "\n--- REKOMENDASI TOP " << n << " PRODUK TERLARIS (MAX-HEAP) ---\n";
    int batas = min(n, (int)heap.size());
    for (int i = 0; i < batas; i++) {
        ProdukCount top = extractMax(heap);
        cout << i + 1 << ". " << top.nama_produk
             << " (Terjual: " << top.frekuensi << " item)\n";
    }
}

// Rekomendasi Top-N menggunakan Sort (pembanding)
void generateTopN(const vector<Transaksi>& dataset, int n) {
    unordered_map<string, int> freq;
    for (const auto& t : dataset)
        freq[t.nama_produk] += t.jumlah_pembelian;

    vector<ProdukCount> list;
    for (const auto& p : freq)
        list.push_back({p.first, p.second});

    sort(list.begin(), list.end(), compareProduk);

    cout << "\n--- REKOMENDASI TOP " << n << " PRODUK TERLARIS (SORT) ---\n";
    int batas = min(n, (int)list.size());
    for (int i = 0; i < batas; i++)
        cout << i + 1 << ". " << list[i].nama_produk
             << " (Terjual: " << list[i].frekuensi << " item)\n";
}

// Pencarian transaksi pelanggan (Linear Search)
void searchByPelanggan_Linear(const vector<Transaksi>& dataset, const string& target) {
    cout << "\n--- HASIL PENCARIAN LINEAR UNTUK PELANGGAN: " << target << " ---\n";
    int count = 0;
    for (const auto& t : dataset) {
        if (t.id_pelanggan == target) {
            cout << "  - " << t.id_transaksi
                 << " | " << t.tanggal_transaksi
                 << " | " << t.nama_produk
                 << " (Qty: " << t.jumlah_pembelian << ")\n";
            count++;
        }
    }
    cout << (count == 0 ? "  Tidak ada transaksi ditemukan.\n"
                        : "  Total: " + to_string(count) + " transaksi.\n");
}

// Pencarian transaksi pelanggan (BST Search)
void searchByPelanggan_BST(NodeBST* root, const string& target) {
    cout << "\n--- HASIL PENCARIAN BST UNTUK PELANGGAN: " << target << " ---\n";
    NodeBST* hasil = searchBST(root, target);
    if (hasil == nullptr) {
        cout << "  Tidak ada transaksi ditemukan.\n";
        return;
    }
    int count = 0;
    for (const auto& t : hasil->transaksi_list) {
        cout << "  - " << t.id_transaksi
             << " | " << t.tanggal_transaksi
             << " | " << t.nama_produk
             << " (Qty: " << t.jumlah_pembelian << ")\n";
        count++;
    }
    cout << "  Total: " << count << " transaksi.\n";
}

// Pencarian berdasarkan ID produk
void searchByProduk(const vector<Transaksi>& dataset, const string& target) {
    cout << "\n--- HASIL PENCARIAN UNTUK PRODUK: " << target << " ---\n";
    int count = 0;
    for (const auto& t : dataset) {
        if (t.id_produk == target) {
            cout << "  - " << t.id_transaksi
                 << " | Pelanggan: " << t.id_pelanggan
                 << " | " << t.tanggal_transaksi << "\n";
            count++;
        }
    }
    cout << (count == 0 ? "  Produk tidak ditemukan.\n"
                        : "  Total: " + to_string(count) + " transaksi.\n");
}

// Tambah transaksi baru (ke vector + BST)
void insertTransaksi(vector<Transaksi>& dataset, NodeBST*& root) {
    Transaksi t;
    cout << "\n--- TAMBAH TRANSAKSI BARU ---\n";
    cout << "  ID Transaksi    : "; cin >> t.id_transaksi;
    cout << "  ID Pelanggan    : "; cin >> t.id_pelanggan;
    cout << "  ID Produk       : "; cin >> t.id_produk;
    cin.ignore();
    cout << "  Nama Produk     : "; getline(cin, t.nama_produk);
    cout << "  Kategori Produk : "; getline(cin, t.kategori_produk);
    cout << "  Jumlah Pembelian: "; cin >> t.jumlah_pembelian;
    cout << "  Tanggal (YYYY-MM-DD HH:MM): ";
    cin.ignore();
    getline(cin, t.tanggal_transaksi);

    dataset.push_back(t);
    root = insertBST(root, t);
    cout << "  Transaksi berhasil ditambahkan! Total data: " << dataset.size() << "\n";
}

// Perbandingan waktu eksekusi Linear vs BST
void bandingkanPencarian(const vector<Transaksi>& dataset, NodeBST* root, const string& target) {
    cout << "\n--- PERBANDINGAN PENCARIAN: LINEAR vs BST untuk [" << target << "] ---\n";

    auto t0 = high_resolution_clock::now();
    searchByPelanggan_Linear(dataset, target);
    auto t1 = high_resolution_clock::now();
    cout << "  Waktu Linear: " << duration_cast<microseconds>(t1 - t0).count() << " mikrodetik\n";

    auto t2 = high_resolution_clock::now();
    searchByPelanggan_BST(root, target);
    auto t3 = high_resolution_clock::now();
    cout << "  Waktu BST   : " << duration_cast<microseconds>(t3 - t2).count() << " mikrodetik\n";
}

// Info konsumsi memori estimasi
void tampilkanMemori(const vector<Transaksi>& dataset, NodeBST* root) {
    long long mem_vec      = dataset.size() * sizeof(Transaksi);
    int       node_unik    = countNodesBST(root);
    long long overhead_bst = node_unik * sizeof(NodeBST);
    long long mem_bst      = mem_vec + overhead_bst;

    cout << "\n--- ESTIMASI KONSUMSI MEMORI ---\n";
    cout << "  Vector (baseline) : " << mem_vec      << " bytes (" << mem_vec / 1024      << " KB)\n";
    cout << "  BST (vector+node) : " << mem_bst      << " bytes (" << mem_bst / 1024      << " KB)\n";
    cout << "  Overhead node BST : " << overhead_bst << " bytes (" << overhead_bst / 1024 << " KB)\n";
    cout << "  Jumlah node unik  : " << node_unik    << " pelanggan\n";
}

// ==========================================
// BAGIAN 6 — MAIN PROGRAM & MENU CLI
// ==========================================

void tampilkanMenu() {
    cout << "\n+==================================================+\n";
    cout << "|         SISTEM REKOMENDASI TRANSAKSI             |\n";
    cout << "+==================================================+\n";
    cout << "|  [1]  Muat ulang data (ganti jumlah transaksi)   |\n";
    cout << "|  [2]  Rekomendasi Top-N produk (Max-Heap)        |\n";
    cout << "|  [3]  Rekomendasi Top-N produk (Sort)            |\n";
    cout << "|  [4]  Cari transaksi pelanggan (Linear Search)   |\n";
    cout << "|  [5]  Cari transaksi pelanggan (BST Search)      |\n";
    cout << "|  [6]  Bandingkan Linear vs BST (input pelanggan) |\n";
    cout << "|  [7]  Cari transaksi berdasarkan ID produk       |\n";
    cout << "|  [8]  Tambah transaksi baru                      |\n";
    cout << "|  [9]  Tampilkan info memori                      |\n";
    cout << "|  [0]  Keluar                                     |\n";
    cout << "+==================================================+\n";
    cout << "Pilih menu: ";
}

int main() {
    vector<Transaksi> dataset;
    NodeBST* root   = nullptr;
    bool     loaded = false;

    cout << "=== SISTEM REKOMENDASI TRANSAKSI - BST & MAX-HEAP ===\n";

    // Muat data awal
    int jumlah;
    cout << "Masukkan jumlah data awal yang ingin dimuat (-1 = semua): ";
    cin >> jumlah;

    auto t0 = high_resolution_clock::now();
    loadData("dataset_rekomendasi_final.csv", dataset, jumlah);
    auto t1 = high_resolution_clock::now();
    cout << "Waktu load: " << duration_cast<microseconds>(t1 - t0).count() << " mikrodetik\n";

    // Bangun BST
    cout << "\nMembangun BST...\n";
    auto t2 = high_resolution_clock::now();
    root    = buildBST(dataset);
    auto t3 = high_resolution_clock::now();
    cout << "BST selesai dibangun. Waktu: "
         << duration_cast<microseconds>(t3 - t2).count() << " mikrodetik\n";
    loaded = true;

    int pilihan;
    do {
        tampilkanMenu();
        if (!(cin >> pilihan)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            pilihan = -1;
        }

        if (!loaded && pilihan != 1 && pilihan != 0) {
            cout << "Data belum dimuat. Pilih menu [1] terlebih dahulu.\n";
            continue;
        }

        switch (pilihan) {

        case 1: {
            // Muat ulang data
            dataset.clear();
            root = nullptr;
            cout << "Masukkan jumlah transaksi yang ingin dimuat (-1 = semua): ";
            cin >> jumlah;
            auto s = high_resolution_clock::now();
            loadData("dataset_rekomendasi_final.csv", dataset, jumlah);
            auto e = high_resolution_clock::now();
            cout << "Waktu load: " << duration_cast<microseconds>(e - s).count() << " mikrodetik\n";
            cout << "Membangun ulang BST...\n";
            root   = buildBST(dataset);
            loaded = true;
            cout << "BST berhasil dibangun ulang.\n";
            break;
        }

        case 2: {
            // Rekomendasi Top-N Max-Heap
            int n;
            cout << "Masukkan nilai N (jumlah produk teratas): ";
            cin >> n;
            auto s = high_resolution_clock::now();
            generateTopN_Heap(dataset, n);
            auto e = high_resolution_clock::now();
            cout << "Waktu eksekusi: " << duration_cast<microseconds>(e - s).count() << " mikrodetik\n";
            break;
        }

        case 3: {
            // Rekomendasi Top-N Sort
            int n;
            cout << "Masukkan nilai N (jumlah produk teratas): ";
            cin >> n;
            auto s = high_resolution_clock::now();
            generateTopN(dataset, n);
            auto e = high_resolution_clock::now();
            cout << "Waktu eksekusi: " << duration_cast<microseconds>(e - s).count() << " mikrodetik\n";
            break;
        }

        case 4: {
            // Linear search pelanggan
            string id;
            cout << "Masukkan ID Pelanggan (contoh: CUST_016): ";
            cin >> id;
            auto s = high_resolution_clock::now();
            searchByPelanggan_Linear(dataset, id);
            auto e = high_resolution_clock::now();
            cout << "Waktu eksekusi: " << duration_cast<microseconds>(e - s).count() << " mikrodetik\n";
            break;
        }

        case 5: {
            // BST search pelanggan
            string id;
            cout << "Masukkan ID Pelanggan (contoh: CUST_016): ";
            cin >> id;
            auto s = high_resolution_clock::now();
            searchByPelanggan_BST(root, id);
            auto e = high_resolution_clock::now();
            cout << "Waktu eksekusi: " << duration_cast<microseconds>(e - s).count() << " mikrodetik\n";
            break;
        }

        case 6: {
            // Bandingkan Linear vs BST
            string id;
            cout << "Masukkan ID Pelanggan untuk perbandingan: ";
            cin >> id;
            bandingkanPencarian(dataset, root, id);
            break;
        }

        case 7: {
            // Cari berdasarkan ID produk
            string id;
            cout << "Masukkan ID Produk (contoh: PRD_102): ";
            cin >> id;
            searchByProduk(dataset, id);
            break;
        }

        case 8:
            // Tambah transaksi
            insertTransaksi(dataset, root);
            break;

        case 9:
            // Info memori
            tampilkanMemori(dataset, root);
            break;

        case 0:
            cout << "\nProgram selesai\n";
            break;

        default:
            cout << "Pilihan tidak valid.\n";
        }

    } while (pilihan != 0);

    return 0;
}