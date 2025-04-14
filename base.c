#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Sistem sıcaklığını ölçen bir fonksiyon (örnek)
int get_cpu_temperature() {
    // Gerçek sistemlerde bu bilgi bir API veya sistem dosyasından alınır.
    return 75; // Örnek sıcaklık
}

// Fan hızını ayarlayan bir fonksiyon (örnek)
void set_fan_speed(int speed) {
    printf("Fan hızı %d%% olarak ayarlandı.\n", speed);
}

// Gereksiz işlemleri kapatan bir fonksiyon (örnek)
void optimize_system() {
    printf("Gereksiz işlemler optimize ediliyor...\n");
    // Örnek: Arka planda çalışan yazılımları kapatma
    system("taskkill /F /IM unnecessary_process.exe");
}

// Bilgisayar hızını artıran bir fonksiyon (örnek)
void overclock_cpu() {
    printf("İşlemci hızı artırılıyor...\n");
    // Not: Gerçek overclock işlemleri donanım seviyesinde yapılır.
    // Bu sadece bir simülasyondur.
}

int main() {
    printf("Bilgisayar optimizasyon aracı başlatılıyor...\n");

    // Sıcaklık kontrolü
    int temperature = get_cpu_temperature();
    if (temperature > 70) {
        printf("Sıcaklık çok yüksek (%d°C). Fan hızını artırıyorum...\n", temperature);
        set_fan_speed(100);
    } else {
        printf("Sıcaklık normal (%d°C).\n", temperature);
    }

    // Sistem optimizasyonu
    optimize_system();

    // İşlemci hızlandırma
    overclock_cpu();

    printf("Bilgisayar optimizasyonu tamamlandı.\n");
    return 0;
}
