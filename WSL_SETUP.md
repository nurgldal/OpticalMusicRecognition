# WSL Kamera Kurulum ve Sorun Giderme Rehberi

Bu proje, Linux ortamında geliştirilmiştir. Eğer Windows üzerinde **WSL (Windows Subsystem for Linux)** kullanarak projeyi derliyorsanız, laptop kamerasını veya USB web kameranızı WSL'e tanıtmak için aşağıdaki adımları uygulamanız gerekir.

> **Not:** Windows'un güvenlik yapısı gereği WSL, donanımlara (kamera, USB vb.) varsayılan olarak erişemez. Bu yüzden harici bir köprü yazılımı gereklidir.

---

## 1. Gerekli Aracın Kurulumu (Windows Tarafı)

Microsoft tarafından desteklenen açık kaynaklı `usbipd-win` aracını kurmalısınız.

1. **PowerShell**'i **Yönetici Olarak (Administrator)** çalıştırın.
2. Aşağıdaki komutu yapıştırıp kurulumu yapın:
   " winget install --interactive --exact dorssel.usbipd-win "

3. Kurulum bittikten sonra açık olan tüm terminalleri kapatıp yeniden açın.

## 2. Kamerayı WSL'e Bağlama
1. Projeyi çalıştırmadan önce kamerayı Linux tarafına "attach" etmelisiniz.

PowerShell (Yönetici) açın ve bağlı USB cihazlarını listeleyin:
" usbipd list "
(Listede kameranızı bulun. Örneğin: "Integrated Webcam" veya "USB Video Device". Yanındaki BUSID'yi (örn: 1-7 not edin.)

2. Kamerayı WSL'e bağlayın (BUSID kısmını kendi numaranızla değiştirin):
# Sadece ilk seferde gerekli:
usbipd bind --busid 1-4

# Her bilgisayar açılışında veya kamera koptuğunda gerekli:
usbipd attach --wsl --busid 1-4

3. Kontrol: Ubuntu terminaline gidip şu komutu yazın:
" ls -l /dev/video* "
Eğer /dev/video0 dosyasını görüyorsanız kamera başarıyla bağlanmıştır.

## 3. İzin Ayarları (Linux Tarafı)
1. Ubuntu terminalinde şu komutu çalıştırın:
" sudo usermod -aG video $USER "
Şifre isteyebilir.

2. Ayarın aktif olması için terminali kapatıp açın veya şu komutu girin:
" newgrp video "

