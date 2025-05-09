# Tugas Pemrograman A - Kelompok 7

## Deskripsi
Program ini digunakan untuk melakukan analisis data populasi dan persentase pengguna internet di Indonesia dari tahun 1960 hingga 2023. Program melakukan interpolasi dan regresi polinomial untuk:
- Mengestimasi data yang hilang pada tahun 2005, 2006, 2015, dan 2016.
- Membentuk persamaan polinomial untuk populasi dan persentase pengguna internet.
- Melakukan prediksi populasi tahun 2030 dan jumlah pengguna internet tahun 2035.
- Menyediakan visualisasi data dan hasil fitting menggunakan ASCII plot dan gnuplot.

## File
- `CVers.c` : Source code implementasi C untuk analisis data.
- `pythonVers.py` : Source code implementasi Python untuk analisis data.
- `data.csv` : Data populasi dan persentase pengguna internet Indonesia.
- `plot_script.gnu` : Skrip gnuplot yang dihasilkan untuk visualisasi.

## Cara Menjalankan
1. **Untuk program C:**
   ```bash
   gcc CVers.c -o CVers -lm
   ./CVers
   ```
   * Jika diminta, masukkan path ke executable GNUplot (contoh: C:\Program Files\gnuplot\bin\gnuplot.exe)

2. **Untuk program Python:**
   ```bash
   python pythonVers.py
   ```

## Output
- Hasil estimasi dan persamaan polinomial akan ditampilkan di terminal.
- Visualisasi data dan kurva fitting disimpan dalam file PNG.

## Catatan
- Pastikan file `data.csv` sudah tersedia dan sesuai format (tahun, persentase_internet, populasi).
- Program ini menggunakan regresi polinomial dengan derajat terbaik berdasarkan nilai R².
- Implementasi C menggunakan GNUplot untuk visualisasi, sehingga perlu diinstal terlebih dahulu.
- Implementasi Python menggunakan matplotlib untuk visualisasi.

## Anggota Kelompok 7
1. Andi Muhammad Alvin - 2306161933
2. Ryan Adidaru Barnabi - 2306266994 
3. Rowen Rodotua Harahap - 2306250604

## Lisensi
Program ini dibuat untuk keperluan tugas kuliah dan bebas digunakan untuk pembelajaran.
