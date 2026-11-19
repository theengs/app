# Mi A2 üzerinde Theengs Android BLE→MQTT kurulumu

Bu fork, Theengs'in mevcut GPL açık kaynak kodunu kullanır. Telefonla veya sensörlerle bu CI çalışması sırasında hiçbir bağlantı kurulmaz.

## APK kurulunca

1. GitHub Actions artifact içindeki `Theengs-arm64-v8a.apk` dosyasını Mi A2'ye aktar ve Android 10'a kur.
2. İlk açılışta Bluetooth iznini ver. Android 10 için konum izni BLE taraması açısından gereklidir; uygulama isterse **Kullanırken izin ver** ve sürekli arka plan taraması için sistemin sunduğu arka plan konum iznini seç.
3. Android ayarlarında uygulama için pil optimizasyonunu kaldır. Uygulamanın bildirim izni/foreground service bildirimi gösterimine izin ver.
4. Uygulama içinden MQTT'yi aç:
   - Broker adresi: daha sonra kurulacak yerel broker'ın adresi
   - Port: TLS yoksa 1883; TLS kullanılırsa broker yapılandırmasına göre 8883
   - Kullanıcı adı/şifre: yalnızca broker için oluşturulmuş ayrı hesap
   - Discovery: açık bırak
   - Topic A/B: varsayılan `home/TheengsApp` kullanılabilir
5. BLE cihaz taramasını başlat ve LYWSD03MMC cihazlarını adları/MAC adresleriyle kontrol et.

## MQTT ve Home Assistant

Bu görev broker kurmaz ve Home Assistant yapılandırmasını değiştirmez. Aynı Mi A2 üzerinde yerel bir MQTT broker daha sonra ayrıca kurulmalıdır. Broker yalnızca yerel ağ arayüzüne bağlanmalı; WAN'a açılmamalı ve güçlü, ayrı bir kullanıcı hesabı kullanılmalıdır. Theengs uygulamasına kişisel parola gömülmez.

Veri akışı:

`LYWSD03MMC → Android Bluetooth → Theengs → MQTT broker → Home Assistant`

Kaynakta Home Assistant MQTT Discovery desteği mevcuttur. Discovery açıkken cihaz/özellik yapılandırmaları `homeassistant/sensor/.../config` konularında yayınlanır; ölçüm verileri varsayılan olarak `home/TheengsApp/BTtoMQTT/<cihaz>` biçiminde yayınlanır. Discovery çalışmazsa aynı JSON verilerinden elle MQTT sensor entity tanımlamak mümkündür; önce broker'da gerçek topic/payload görülmelidir.

## Xiaomi sensörleri

LYWSD03MMC, Theengs README'sinde desteklenen Xiaomi sensörleri arasında listelenmiştir. Sadece reklam paketinden sıcaklık, bağıl nem, pil ve RSSI verisi alınır. Şifreli/özel firmware reklamı varsa bind key gerekebilir; bu projede anahtar üretilmez, tahmin edilmez ve sensör firmware'i değiştirilmez.

## Android 10 süreklilik notu

Manifest ve kaynakta boot receiver, foreground service, wake lock, exact-alarm ve Android BLE izinleri bulunur. Buna rağmen Android üretici güç yönetimi taramayı kısıtlayabilir. 24/7 davranış fiziksel telefonda test edilmeden garanti edilemez. Mi A2'de pil optimizasyonunu kaldırmak ve otomatik başlatma izinlerini kontrol etmek gerekir.

## Geri alma

Uygulama içinden MQTT'yi kapatıp uygulamayı durdur. Sonra Android **Ayarlar → Uygulamalar → Theengs BLE → Kaldır** yoluyla yalnızca bu test APK'sını kaldır. Bu işlem Home Assistant, Termux, AdGuard Home, Magisk veya diğer uygulamalara dokunmaz.