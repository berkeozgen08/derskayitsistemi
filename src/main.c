#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KOD_UZUNLUGU 8
#define AD_UZUNLUGU 50
#define NUMARA_UZUNLUGU 9
#define TARIH_UZUNLUGU 11
#define BUFFER_BUYUKLUGU 256
#define DERS_BASLIK "%-15s%-50s%-15s%-15s\n", "KOD", "AD", "KREDI", "KONTENJAN"
#define OGRENCI_BASLIK "%-15s%-25s%-25s%-15s%-15s\n", "NUMARA", "AD", "SOYAD", "KREDI SAYISI", "DERS SAYISI"
#define KAYIT_BASLIK "%-10s%-15s%-15s%-15s%-15s\n", "ID", "KOD", "NUMARA", "TARIH", "DURUM"
#define KAYITLI "kayitli"
#define DERS_KAPANDI "ders kapandi"
#define BIRAKTI "sildi"
#define DERS_DOSYASI "dersler.txt"
#define OGRENCI_DOSYASI "ogrenciler.txt"
#define KAYIT_DOSYASI "OgrenciDersKayit.txt"

typedef struct Ders {
	char *kod;
	char *ad;
	int kredi;
	int kontenjan;
	struct Ders *next;
	char **kayitli;
	int kayitli_sayisi;
} Ders;

typedef struct Ogrenci {
	char *numara;
	char *ad;
	char *soyad;
	int kredi_sayisi;
	int ders_sayisi;
	struct Ogrenci *next;
	struct Ogrenci *prev;
} Ogrenci;

typedef struct Kayit {
	int id;
	char *kod;
	char *numara;
	char *tarih;
	char *durum;
	struct Kayit *next;
} Kayit;

Ders *dersYarat();
Ders *dersGir();
Ders *dersBul(Ders *dersler, char *kod);
Ders *dersEkle(Ders *dersler, Ders *yeni);
Ders *dersSil(Ders *dersler, char *kod);
void dersiYazdir(Ders *ders, FILE *stream);
void dersleriYazdir(Ders *dersler);
void dersleriFreele(Ders *dersler);

Ogrenci *ogrenciYarat();
Ogrenci *ogrenciGir();
Ogrenci *ogrenciBul(Ogrenci *ogrenciler, char *numara);
Ogrenci *ogrenciEkle(Ogrenci *ogrenciler, Ogrenci *yeni);
Ogrenci *ogrenciSil(Ogrenci *ogrenciler, char *numara);
void ogrenciYazdir(Ogrenci *ogrenci, FILE *stream);
void ogrencileriYazdir(Ogrenci *ogrenciler);
void ogrencileriFreele(Ogrenci *ogrenciler);

Kayit *kayitYarat();
Kayit *kayitBul(Kayit *kayitlar, char *numara, char *kod);
Kayit *kayitEkleGuncelle(Kayit *kayitlar, Kayit *yeni);
Kayit *kayitSil(Kayit *kayitlar, char *numara, char *kod);
void kayitYazdir(Kayit *kayit, FILE *stream);
void kayitlariYazdir(Kayit *kayitlar);
void kayitlariFreele(Kayit *kayitlar);

/* DOSYADAN OKUMA ISLEMLERI */
void *dersleriOku(void *liste, char *buffer);
void *ogrencileriOku(void *liste, char *buffer);
void *kayitlariOku(void *liste, char *buffer);
void *dosyaOku(char *dosya, void *(*oku)(void *, char *));

/* DOSYAYA YAZMA ISLEMLERI */
void *dersleriYaz(void *liste, FILE *f);
void *ogrencileriYaz(void *liste, FILE *f);
void *kayitlariYaz(void *liste, FILE *f);
void *dosyaYaz(void *liste, char *dosya, void *(*yaz)(void *, FILE *));

int derseOgrenciKaydet(Ders *ders, char *numara);
int derstenOgrenciKaydiSil(Ders *ders, char *numara);
Kayit *ogrenciKayitlariniSil(Kayit *kayitlar, Ders *dersler, char *numara);
void kayitlariDerslereIsle(Kayit *kayitlar, Ders *dersler);

Kayit *ogrenciKaydetmeKaydi(Ogrenci *ogrenci, Ders *ders, char *tarih, int maks_kredi, int maks_ders);
Kayit *dersKapatmaKaydi(Kayit *kayitlar, Ders *ders, Ogrenci *ogrenciler, char *tarih);
Kayit *ogrenciDersBirakmaKaydi(Ogrenci *ogrenci, Ders *ders, char *tarih);

/* 8, 9 */
void sinifListesiCikar(Ders *dersler, Ogrenci *ogrenciler, char *kod);
void dersProgramiCikar(Ogrenci *ogrenciler, Ders *dersler, char *numara);

int main() {
	Ders *dersler;
	Ogrenci *ogrenciler;
	Kayit *kayitlar;
	char *kod;
	char *numara;
	char *tarih;
	int maks_kredi;
	int maks_ders;
	int choice;

	dersler = (Ders *) dosyaOku(DERS_DOSYASI, dersleriOku);
	ogrenciler = (Ogrenci *) dosyaOku(OGRENCI_DOSYASI, ogrencileriOku);
	kayitlar = (Kayit *) dosyaOku(KAYIT_DOSYASI, kayitlariOku);
	kayitlariDerslereIsle(kayitlar, dersler);
	kod = (char *) malloc(KOD_UZUNLUGU * sizeof(char));
	numara = (char *) malloc(NUMARA_UZUNLUGU * sizeof(char));
	tarih = (char *) malloc(TARIH_UZUNLUGU * sizeof(char));
	printf("Alinabilecek maksimum kredi: ");
	scanf("%d", &maks_kredi);
	printf("Alinabilecek maksimum ders: ");
	scanf("%d", &maks_ders);
	choice = -1;

	while (choice) {
		printf("0. Cikis\n1. Ders ac\n2. Ders kapat\n3. Dersleri listele\n4. Ogrenci kaydet\n5. Ogrenci kaydini sil\n6. Ogrencileri listele\n7. Derse ogrenci kaydet\n8. Dersten ogrenci kaydi sil\n9. Ders kayitlarini listele\n10. Sinif listesi cikar\n11. Ogrenci ders programi cikar\nSecim: ");
		scanf("%d", &choice);

		switch (choice) {
			case 1:
				dersler = dersEkle(dersler, dersGir());
				dosyaYaz(dersler, DERS_DOSYASI, dersleriYaz);
				break;
			case 2:
				printf("Ders kodu: ");
				scanf("%s", kod);
				printf("Kayit tarihi: ");
				scanf("%s", tarih);
				kayitlar = dersKapatmaKaydi(kayitlar, dersBul(dersler, kod), ogrenciler, tarih);
				dersler = dersSil(dersler, kod);
				dosyaYaz(kayitlar, KAYIT_DOSYASI, kayitlariYaz);
				dosyaYaz(dersler, DERS_DOSYASI, dersleriYaz);
				break;
			case 3:
				printf(DERS_BASLIK);
				dersleriYazdir(dersler);
				break;
			case 4:
				ogrenciler = ogrenciEkle(ogrenciler, ogrenciGir());
				dosyaYaz(ogrenciler, OGRENCI_DOSYASI, ogrencileriYaz);
				break;
			case 5:
				printf("Ogrenci numarasi: ");
				scanf("%s", numara);
				kayitlar = ogrenciKayitlariniSil(kayitlar, dersler, numara);
				ogrenciler = ogrenciSil(ogrenciler, numara);
				dosyaYaz(kayitlar, KAYIT_DOSYASI, kayitlariYaz);
				dosyaYaz(ogrenciler, OGRENCI_DOSYASI, ogrencileriYaz);
				break;
			case 6:
				printf(OGRENCI_BASLIK);
				ogrencileriYazdir(ogrenciler);
				break;
			case 7:
				printf("Ders kodu: ");
				scanf("%s", kod);
				printf("Ogrenci numarasi: ");
				scanf("%s", numara);
				printf("Kayit tarihi: ");
				scanf("%s", tarih);
				kayitlar = kayitEkleGuncelle(kayitlar, ogrenciKaydetmeKaydi(ogrenciBul(ogrenciler, numara), dersBul(dersler, kod), tarih, maks_kredi, maks_ders));
				dosyaYaz(kayitlar, KAYIT_DOSYASI, kayitlariYaz);
				dosyaYaz(ogrenciler, OGRENCI_DOSYASI, ogrencileriYaz);
				break;
			case 8:
				printf("Ders kodu: ");
				scanf("%s", kod);
				printf("Ogrenci numarasi: ");
				scanf("%s", numara);
				printf("Kayit tarihi: ");
				scanf("%s", tarih);
				kayitlar = kayitEkleGuncelle(kayitlar, ogrenciDersBirakmaKaydi(ogrenciBul(ogrenciler, numara), dersBul(dersler, kod), tarih));
				dosyaYaz(kayitlar, KAYIT_DOSYASI, kayitlariYaz);
				dosyaYaz(ogrenciler, OGRENCI_DOSYASI, ogrencileriYaz);
				break;
			case 9:
				printf(KAYIT_BASLIK);
				kayitlariYazdir(kayitlar);
				break;
			case 10:
				printf("Ders kodu: ");
				scanf("%s", kod);
				sinifListesiCikar(dersler, ogrenciler, kod);
				break;
			case 11:
				printf("Ogrenci numarasi: ");
				scanf("%s", numara);
				dersProgramiCikar(ogrenciler, dersler, numara);
		}
	}

	free(tarih);
	free(numara);
	free(kod);
	kayitlariFreele(kayitlar);
	ogrencileriFreele(ogrenciler);
	dersleriFreele(dersler);
	return 0;
}

Ders *dersYarat() {
	Ders *ders;

	ders = (Ders *) malloc(sizeof(Ders));
	ders->kod = (char *) malloc(KOD_UZUNLUGU * sizeof(char));
	ders->ad = (char *) malloc(AD_UZUNLUGU * sizeof(char));
	ders->kredi = 0;
	ders->kontenjan = 0;
	ders->next = NULL;
	ders->kayitli = (char **) malloc(0);
	ders->kayitli_sayisi = 0;

	return ders;
}

Ders *dersGir() {
	Ders *ders;
	
	ders = dersYarat();
	printf("Ders kodu: ");
	scanf("%s", ders->kod);
	printf("Ders adi: ");
	scanf(" %[^\n]s", ders->ad);
	printf("Ders kredisi: ");
	scanf(" %d", &ders->kredi);
	printf("Ders kontenjani: ");
	scanf(" %d", &ders->kontenjan);

	return ders;
}

Ders *dersBul(Ders *dersler, char *kod) {
	Ders *iter;

	iter = dersler;

	while (iter != NULL && strcmp(kod, iter->kod)) {
		iter = iter->next;
	}

	return iter;
}

Ders *dersEkle(Ders *dersler, Ders *yeni) {
	Ders *prev;
	Ders *iter;

	if (dersler == NULL) return yeni;
	if (yeni == NULL) return dersler;

	prev = NULL;
	iter = dersler;

	while (iter != NULL && strcmp(yeni->kod, iter->kod) > 0) {
		prev = iter;
		iter = iter->next;
	}

	if (iter != NULL && !strcmp(yeni->kod, iter->kod)) {
		printf("%s kodlu ders zaten acilmis.\n", yeni->kod);
		dersleriFreele(yeni);
		return dersler;
	} else if (prev == NULL) {
		yeni->next = dersler;
		return yeni;
	} else {
		prev->next = yeni;
		yeni->next = iter;
		return dersler;
	}
}

Ders *dersSil(Ders *dersler, char *kod) {
	Ders *prev;
	Ders *iter;

	prev = NULL;
	iter = dersler;

	while (iter != NULL && strcmp(kod, iter->kod)) {
		prev = iter;
		iter = iter->next;
	}

	if (iter == NULL) {
		printf("%s kodlu ders acilmamis.\n", kod);
		return dersler;
	} else if (prev == NULL) {
		prev = iter->next;
		iter->next = NULL;
		dersleriFreele(iter);
		return prev;
	} else {
		prev->next = iter->next;
		iter->next = NULL;
		dersleriFreele(iter);
		return dersler;
	}
}

void dersiYazdir(Ders *ders, FILE *stream) {
	fprintf(stream, "%-15s%-50s%-15d%-15d\n", ders->kod, ders->ad, ders->kredi, ders->kontenjan);
}

void dersleriYazdir(Ders *dersler) {
	if (dersler != NULL) {
		dersiYazdir(dersler, stdout);
		dersleriYazdir(dersler->next);
	}
}

void dersleriFreele(Ders *dersler) {
	if (dersler != NULL) {
		dersleriFreele(dersler->next);
		free(dersler->kod);
		free(dersler->ad);
		free(dersler->kayitli);
		free(dersler);
	}
}

Ogrenci *ogrenciYarat() {
	Ogrenci *ogrenci;

	ogrenci = (Ogrenci *) malloc(sizeof(Ogrenci));
	ogrenci->numara = (char *) malloc(NUMARA_UZUNLUGU * sizeof(char));
	ogrenci->ad = (char *) malloc(AD_UZUNLUGU * sizeof(char));
	ogrenci->soyad = (char *) malloc(AD_UZUNLUGU * sizeof(char));
	ogrenci->kredi_sayisi = 0;
	ogrenci->ders_sayisi = 0;
	ogrenci->next = NULL;
	ogrenci->prev = NULL;

	return ogrenci;
}

Ogrenci *ogrenciGir() {
	Ogrenci *ogrenci;

	ogrenci = ogrenciYarat();
	printf("Ogrenci numarasi: ");
	scanf("%s", ogrenci->numara);
	printf("Ogrenci adi: ");
	scanf("%s", ogrenci->ad);
	printf("Ogrenci soyadi: ");
	scanf("%s", ogrenci->soyad);

	return ogrenci;
}

Ogrenci *ogrenciBul(Ogrenci *ogrenciler, char *numara) {
	Ogrenci *iter;

	iter = ogrenciler;

	while (iter != NULL && strcmp(numara, iter->numara)) {
		iter = iter->next;
	}

	return iter;
}

Ogrenci *ogrenciEkle(Ogrenci *ogrenciler, Ogrenci *yeni) {
	Ogrenci *prev;
	Ogrenci *iter;

	if (ogrenciler == NULL) return yeni;
	if (yeni == NULL) return ogrenciler;

	prev = NULL;
	iter = ogrenciler;

	while (iter != NULL && strcmp(yeni->numara, iter->numara) > 0) {
		prev = iter;
		iter = iter->next;
	}

	if (iter != NULL && !strcmp(yeni->numara, iter->numara)) {
		printf("%s numarali ogrenci zaten kayitli.\n", yeni->numara);
		ogrencileriFreele(yeni);
		return ogrenciler;
	} else if (prev == NULL) {
		yeni->next = ogrenciler;
		return yeni;
	} else {
		prev->next = yeni;
		yeni->prev = prev;
		yeni->next = iter;
		if (iter != NULL) {
			iter->prev = yeni;
		}
		return ogrenciler;
	}
}

Ogrenci *ogrenciSil(Ogrenci *ogrenciler, char *numara) {
	Ogrenci *ogrenci;
	Ogrenci *temp;

	ogrenci = ogrenciBul(ogrenciler, numara);

	if (ogrenci == NULL) {
		printf("%s numarali ogrenci kayitli degil.\n", numara);
		return ogrenciler;
	} else if (ogrenci->prev == NULL) {
		temp = ogrenci->next;
		ogrenci->next = NULL;
		ogrencileriFreele(ogrenci);
		return temp;
	} else {
		ogrenci->prev->next = ogrenci->next;
		ogrenci->next->prev = ogrenci->prev;
		ogrenci->next = NULL;
		ogrencileriFreele(ogrenci);
		return ogrenciler;
	}
}

void ogrenciYazdir(Ogrenci *ogrenci, FILE *stream) {
	fprintf(stream, "%-15s%-25s%-25s%-15d%-15d\n", ogrenci->numara, ogrenci->ad, ogrenci->soyad, ogrenci->kredi_sayisi, ogrenci->ders_sayisi);
}

void ogrencileriYazdir(Ogrenci *ogrenciler) {
	if (ogrenciler != NULL) {
		ogrenciYazdir(ogrenciler, stdout);
		ogrencileriYazdir(ogrenciler->next);
	}
}

void ogrencileriFreele(Ogrenci *ogrenciler) {
	if (ogrenciler != NULL) {
		ogrencileriFreele(ogrenciler->next);
		free(ogrenciler->numara);
		free(ogrenciler->ad);
		free(ogrenciler->soyad);
		free(ogrenciler);
	}
}

Kayit *kayitYarat() {
	Kayit *kayit;

	kayit = (Kayit *) malloc(sizeof(Kayit));
	kayit->id = 0;
	kayit->kod = (char *) malloc(KOD_UZUNLUGU * sizeof(char));
	kayit->numara = (char *) malloc(NUMARA_UZUNLUGU * sizeof(char));
	kayit->tarih = (char *) malloc(TARIH_UZUNLUGU * sizeof(char));
	kayit->durum = (char *) malloc(AD_UZUNLUGU * sizeof(char));
	kayit->next = NULL;

	return kayit;
}

Kayit *kayitBul(Kayit *kayitlar, char *numara, char *kod) {
	Kayit *iter;

	iter = kayitlar;

	while (iter != NULL && strcmp(numara, iter->numara) && strcmp(kod, iter->kod)) {
		iter = iter->next;
	}

	return iter;
}

Kayit *kayitEkleGuncelle(Kayit *kayitlar, Kayit *yeni) {
	Kayit *prev;
	Kayit *iter;

	if (kayitlar == NULL) return yeni;
	if (yeni == NULL) return kayitlar;

	prev = NULL;
	iter = kayitlar;

	while (iter != NULL && (strcmp(yeni->numara, iter->numara) || strcmp(yeni->kod, iter->kod))) {
		prev = iter;
		iter = iter->next;
	}

	if (iter != NULL) {
		strcpy(iter->tarih, yeni->tarih);
		strcpy(iter->durum, yeni->durum);
		kayitlariFreele(yeni);
		return kayitlar;
	} else {
		prev->next = yeni;
		yeni->next = NULL;
		yeni->id = prev->id + 1;
		return kayitlar;
	}
}

Kayit *kayitSil(Kayit *kayitlar, char *numara, char *kod) {
	Kayit *prev;
	Kayit *iter;

	prev = NULL;
	iter = kayitlar;

	while (iter != NULL && (strcmp(numara, iter->numara) || strcmp(kod, iter->kod))) {
		prev = iter;
		iter = iter->next;
	}

	if (iter == NULL) {
		printf("%s kodlu derse %s numarali ogrencinin kayit bilgisi yok.\n", kod, numara);
		return kayitlar;
	} else if (prev == NULL) {
		prev = iter->next;
		iter->next = NULL;
		kayitlariFreele(iter);
		return prev;
	} else {
		prev->next = iter->next;
		iter->next = NULL;
		kayitlariFreele(iter);
		return kayitlar;
	}
}

void kayitYazdir(Kayit *kayit, FILE *stream) {
	fprintf(stream, "%-10d%-15s%-15s%-15s%-15s\n", kayit->id, kayit->kod, kayit->numara, kayit->tarih, kayit->durum);
}

void kayitlariYazdir(Kayit *kayitlar) {
	if (kayitlar != NULL) {
		kayitYazdir(kayitlar, stdout);
		kayitlariYazdir(kayitlar->next);
	}
}

void kayitlariFreele(Kayit *kayitlar) {
	if (kayitlar != NULL) {
		kayitlariFreele(kayitlar->next);
		free(kayitlar->numara);
		free(kayitlar->kod);
		free(kayitlar->tarih);
		free(kayitlar->durum);
		free(kayitlar);
	}
}


void *dersleriOku(void *liste, char *buffer) {
	Ders *dersler;
	Ders *yeni;
	char *token;

	dersler = (Ders *) liste;
	yeni = dersYarat();

	token = strtok(buffer, ",");
	strcpy(yeni->kod, token);
	token = strtok(NULL, ",");
	strcpy(yeni->ad, token);
	token = strtok(NULL, ",");
	yeni->kredi = atoi(token);
	token = strtok(NULL, ",");
	yeni->kontenjan = atoi(token);

	dersler = dersEkle(dersler, yeni);
	
	return (void *) dersler;
}

void *ogrencileriOku(void *liste, char *buffer) {
	Ogrenci *ogrenciler;
	Ogrenci *yeni;
	char *token;

	ogrenciler = (Ogrenci *) liste;
	yeni = ogrenciYarat();

	token = strtok(buffer, ",");
	strcpy(yeni->numara, token);
	token = strtok(NULL, ",");
	strcpy(yeni->ad, token);
	token = strtok(NULL, ",");
	strcpy(yeni->soyad, token);
	token = strtok(NULL, ",");
	yeni->kredi_sayisi = atoi(token);
	token = strtok(NULL, ",");
	yeni->ders_sayisi = atoi(token);

	ogrenciler = ogrenciEkle(ogrenciler, yeni);

	return (void *) ogrenciler;
}

void *kayitlariOku(void *liste, char *buffer) {
	Kayit *kayitlar;
	Kayit *yeni;
	char *token;

	kayitlar = (Kayit *) liste;
	yeni = kayitYarat();

	token = strtok(buffer, ",");
	yeni->id = atoi(token);
	token = strtok(NULL, ",");
	strcpy(yeni->kod, token);
	token = strtok(NULL, ",");
	strcpy(yeni->numara, token);
	token = strtok(NULL, ",");
	strcpy(yeni->tarih, token);
	token = strtok(NULL, ",");
	token[strlen(token) - 1] = 0;
	strcpy(yeni->durum, token);

	kayitlar = kayitEkleGuncelle(kayitlar, yeni);

	return (void *) kayitlar;
}

void *dosyaOku(char *dosya, void *(*oku)(void *, char *)) {
	FILE *f;
	void *liste;
	void *yeni;
	char *buffer;

	f = fopen(dosya, "r");
	liste = NULL;
	if (f != NULL) {
		buffer = (char *) malloc(BUFFER_BUYUKLUGU * sizeof(char));

		while (fgets(buffer, BUFFER_BUYUKLUGU, f) != NULL) {
			liste = oku(liste, buffer);
		}

		fclose(f);
		free(buffer);
	}
	return liste;
}


void *dersleriYaz(void *liste, FILE *f) {
	Ders *ders;

	ders = (Ders *) liste;
	fprintf(f, "%s,%s,%d,%d\n", ders->kod, ders->ad, ders->kredi, ders->kontenjan);

	return ders->next;
}

void *ogrencileriYaz(void *liste, FILE *f) {
	Ogrenci *ogrenci;

	ogrenci = (Ogrenci *) liste;
	fprintf(f, "%s,%s,%s,%d,%d\n", ogrenci->numara, ogrenci->ad, ogrenci->soyad, ogrenci->kredi_sayisi, ogrenci->ders_sayisi);

	return ogrenci->next;
}

void *kayitlariYaz(void *liste, FILE *f) {
	Kayit *kayit;

	kayit = (Kayit *) liste;
	fprintf(f, "%d,%s,%s,%s,%s\n", kayit->id, kayit->kod, kayit->numara, kayit->tarih, kayit->durum);

	return kayit->next;
}

void *dosyaYaz(void *liste, char *dosya, void *(*yaz)(void *, FILE *)) {
	FILE *f;
	void *iter;

	f = fopen(dosya, "w");
	iter = liste;

	while (iter != NULL) {
		iter = yaz(iter, f);
	}

	fclose(f);
}


int derseOgrenciKaydet(Ders *ders, char *numara) {
	int i;
	int j;

	i = 0;
	while (i < ders->kayitli_sayisi && strcmp(numara, ders->kayitli[i]) > 0) {
		i++;
	}

	if (i < ders->kayitli_sayisi && !strcmp(numara, ders->kayitli[i])) {
		printf("%s numarali ogrenci %s kodlu derse zaten kayitli.\n", numara, ders->kod);
		return 0;
	} else {
		ders->kayitli = (char **) realloc(ders->kayitli, (ders->kayitli_sayisi + 1) * sizeof(char *));
		for (j = ders->kayitli_sayisi; j > i; j--) {
			ders->kayitli[j] = ders->kayitli[j - 1];
		}
		ders->kayitli[i] = numara;
		ders->kayitli_sayisi++;

		return 1;
	}
}

int derstenOgrenciKaydiSil(Ders *ders, char *numara) {
	int i;

	if (ders == NULL) return 0;

	i = 0;
	while (i < ders->kayitli_sayisi && strcmp(numara, ders->kayitli[i])) {
		i++;
	}

	if (i < ders->kayitli_sayisi) {
		for (; i < ders->kayitli_sayisi - 1; i++) {
			ders->kayitli[i] = ders->kayitli[i + 1];
		}

		ders->kayitli = (char **) realloc(ders->kayitli, (ders->kayitli_sayisi - 1) * sizeof(char *));
		ders->kayitli_sayisi--;

		return 1;
	} else {
		return 0;
	}
}

Kayit *ogrenciKayitlariniSil(Kayit *kayitlar, Ders *dersler, char *numara) {
	Kayit *kayit;
	Kayit *temp;

	kayit = kayitlar;

	while (kayit != NULL) {
		if (!strcmp(numara, kayit->numara)) {
			if (!strcmp(kayit->durum, KAYITLI)) {
				derstenOgrenciKaydiSil(dersBul(dersler, kayit->kod), numara);
			}
			temp = kayit->next;
			kayitlar = kayitSil(kayitlar, numara, kayit->kod);
			kayit = temp;
		} else {
			kayit = kayit->next;
		}
	}

	return kayitlar;
}

void kayitlariDerslereIsle(Kayit *kayitlar, Ders *dersler) {
	Kayit *kayit;
	Ders *ders;

	kayit = kayitlar;

	while (kayit != NULL) {
		if (!strcmp(kayit->durum, KAYITLI)) {
			ders = dersBul(dersler, kayit->kod);

			if (ders != NULL) {
				derseOgrenciKaydet(ders, kayit->numara);
			}
		}
		kayit = kayit->next;
	}
}

Kayit *ogrenciKaydetmeKaydi(Ogrenci *ogrenci, Ders *ders, char *tarih, int maks_kredi, int maks_ders) {
	Kayit *kayit;

	if (ders == NULL) {
		printf("Ders bulunamadi.\n");
	} else if (ogrenci == NULL) {
		printf("Ogrenci bulunamadi.\n");
	} else if (ogrenci->kredi_sayisi + ders->kredi > maks_kredi) {
		printf("Alinabilecek maksimum kredi siniri %d oldugu icin ders alinamaz. Mevcut alinan kredi: %d, dersin kredisi: %d.\n", maks_kredi, ogrenci->kredi_sayisi, ders->kredi);
	} else if (ogrenci->ders_sayisi >= maks_ders) {
		printf("Alinabilecek maksimum ders sayisi %d oldugu icin ders alinamaz. Mevcut ders sayisi: %d.\n", maks_ders, ogrenci->ders_sayisi);
	} else if (ders->kayitli_sayisi >= ders->kontenjan) {
		printf("Dersin %d kisilik kontenjani dolu.\n", ders->kontenjan);
	} else if (derseOgrenciKaydet(ders, ogrenci->numara)) {
		kayit = kayitYarat();
		strcpy(kayit->kod, ders->kod);
		strcpy(kayit->numara, ogrenci->numara);
		strcpy(kayit->tarih, tarih);
		strcpy(kayit->durum, KAYITLI);

		ogrenci->kredi_sayisi += ders->kredi;
		ogrenci->ders_sayisi++;

		return kayit;
	}

	return NULL;
}

Kayit *dersKapatmaKaydi(Kayit *kayitlar, Ders *ders, Ogrenci *ogrenciler, char *tarih) {
	int i;
	Kayit *kayit;
	Ogrenci *ogrenci;

	if (kayitlar == NULL || ders == NULL || ogrenciler == NULL) return kayitlar;

	for (i = 0; i < ders->kayitli_sayisi; i++) {
		ogrenci = ogrenciBul(ogrenciler, ders->kayitli[i]);
		ogrenci->kredi_sayisi -= ders->kredi;
		ogrenci->ders_sayisi--;

		kayit = kayitYarat();
		strcpy(kayit->kod, ders->kod);
		strcpy(kayit->numara, ogrenci->numara);
		strcpy(kayit->tarih, tarih);
		strcpy(kayit->durum, DERS_KAPANDI);
		kayitlar = kayitEkleGuncelle(kayitlar, kayit);
	}

	return kayitlar;
}

Kayit *ogrenciDersBirakmaKaydi(Ogrenci *ogrenci, Ders *ders, char *tarih) {
	Kayit *kayit;

	if (ders == NULL) {
		printf("Ders bulunamadi.\n");
		return NULL;
	}
	if (ogrenci == NULL) {
		printf("Ogrenci bulunamadi.\n");
		return NULL;
	}

	if (derstenOgrenciKaydiSil(ders, ogrenci->numara)) {
		kayit = kayitYarat();
		strcpy(kayit->kod, ders->kod);
		strcpy(kayit->numara, ogrenci->numara);
		strcpy(kayit->tarih, tarih);
		strcpy(kayit->durum, BIRAKTI);

		ogrenci->kredi_sayisi -= ders->kredi;
		ogrenci->ders_sayisi--;

		return kayit;
	} else {
		printf("%s numarali ogrenci %s kodlu dersi almiyor.\n", ogrenci->numara, ders->kod);
		return NULL;
	}
}

void sinifListesiCikar(Ders *dersler, Ogrenci *ogrenciler, char *kod) {
	Ders *ders;
	Ogrenci *ogrenci;
	FILE *f;
	char *dosya;
	int i;

	ders = dersBul(dersler, kod);

	if (ders == NULL) {
		printf("%s kodlu ders acik degil.\n", kod);
	} else if (!ders->kayitli_sayisi) {
		printf("%s kodlu derse kayitli ogrenci yok.\n", kod);
	} else {
		dosya = (char *) malloc((KOD_UZUNLUGU + 4) * sizeof(char));
		sprintf(dosya, "%s.txt", kod);
		f = fopen(dosya, "w");

		fprintf(f, OGRENCI_BASLIK);
		for (i = 0; i < ders->kayitli_sayisi; i++) {
			ogrenci = ogrenciBul(ogrenciler, ders->kayitli[i]);
			ogrenciYazdir(ogrenci, f);
		}

		printf("%s dosyasi olusturuldu.\n", dosya);

		free(dosya);
		fclose(f);
	}
}

void dersProgramiCikar(Ogrenci *ogrenciler, Ders *dersler, char *numara) {
	Ogrenci *ogrenci;
	Ders *ders;
	FILE *f;
	char *dosya;
	int i;
	
	ogrenci = ogrenciBul(ogrenciler, numara);

	if (ogrenci == NULL) {
		printf("%s numarali ogrenci kayitli degil.\n", numara);
	} else if (!ogrenci->ders_sayisi) {
		printf("%s numarali ogrencinin aldigi ders yok.\n", numara);
	} else {
		dosya = (char *) malloc((NUMARA_UZUNLUGU + 17) * sizeof(char));
		sprintf(dosya, "%s_DERSPROGRAMI.txt", numara);
		f = fopen(dosya, "w");
		ders = dersler;

		fprintf(f, DERS_BASLIK);
		while (ders != NULL) {
			i = 0;
			while (i < ders->kayitli_sayisi && strcmp(numara, ders->kayitli[i])) {
				i++;
			}
			if (i < ders->kayitli_sayisi) {
				dersiYazdir(ders, f);
			}
			ders = ders->next;
		}

		printf("%s dosyasi olusturuldu.\n", dosya);

		free(dosya);
		fclose(f);
	}
}
