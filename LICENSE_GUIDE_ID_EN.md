# License / Lisensi

## Bahasa Indonesia

### Lisensi proyek

NUSA UV-K5 APRS DIGI menggunakan **Apache License 2.0**.

Alasannya sederhana: firmware ini merupakan karya turunan/modifikasi dari
ekosistem firmware open-source UV-K5, dan repository dasar yang dipakai
untuk pengembangan REV1G, yaitu:

```text
bg7nzl/uv-k5-v1-firmware
```

juga menggunakan **Apache License 2.0**.

Repository tersebut sendiri merupakan turunan dari pekerjaan open firmware
UV-K5 sebelumnya, termasuk karya DualTachyon dan kontributor lain yang
disebutkan pada repository upstream.

Karena itu, distribusi source code maupun binary NUSA UV-K5 APRS DIGI
tetap mempertahankan lisensi Apache 2.0 dan atribusi upstream.

### Apa yang boleh dilakukan?

Secara umum Apache License 2.0 mengizinkan Anda untuk:

- menggunakan firmware,
- memodifikasi firmware,
- membuat build sendiri,
- mendistribusikan ulang source code,
- mendistribusikan binary,
- menggunakan karya ini dalam proyek lain,
- menggunakan karya ini untuk tujuan pribadi maupun komersial,

selama kewajiban lisensinya dipenuhi.

### Kewajiban utama saat mendistribusikan ulang

Jika Anda mendistribusikan ulang source code atau binary, pertahankan:

1. file `LICENSE`,
2. copyright notice yang relevan,
3. attribution/notice yang relevan,
4. keterangan bahwa file telah dimodifikasi jika memang dimodifikasi,
5. header lisensi yang sudah ada pada source code upstream.

Jangan menghapus nama atau atribusi pembuat upstream dari file yang
memang sudah memilikinya.

### Tentang binary `.packed.bin`

Binary boleh didistribusikan bersama repository/release, namun sangat
disarankan repository tetap menyertakan:

```text
LICENSE
NOTICE
README.md
```

Jika source code nanti ikut dipublikasikan, pertahankan header copyright
dan Apache-2.0 pada file sumber yang berasal dari upstream.

### Modifikasi NUSA

Untuk perubahan yang dibuat dalam proyek NUSA UV-K5 APRS DIGI, atribusi
proyek dapat ditulis sebagai:

```text
Modified work / NUSA UV-K5 APRS DIGI modifications
Copyright 2026 YF9UAG
```

Ini **tidak mengambil alih copyright source upstream**. Copyright upstream
tetap milik masing-masing pemegang haknya.

### Tidak ada garansi

Firmware custom berisiko menyebabkan radio tidak bekerja sebagaimana
diharapkan dan, dalam kondisi tertentu, dapat membutuhkan recovery/reflash.

Apache License 2.0 menyediakan karya ini tanpa jaminan, sejauh diizinkan
oleh hukum yang berlaku.

### File yang disarankan di GitHub

```text
LICENSE
NOTICE
LICENSE_GUIDE_ID_EN.md
```

`LICENSE` sebaiknya berisi teks Apache License 2.0 tanpa diubah.

---

## English

### Project license

NUSA UV-K5 APRS DIGI is distributed under the **Apache License 2.0**.

The reason is that this firmware is a modified/derived work from the
open-source UV-K5 firmware ecosystem. The primary upstream codebase used
during REV1G development:

```text
bg7nzl/uv-k5-v1-firmware
```

is also licensed under the **Apache License 2.0**.

That repository is itself derived from earlier UV-K5 open firmware work,
including DualTachyon's project and other contributors credited by the
upstream repository.

Accordingly, NUSA UV-K5 APRS DIGI keeps the Apache 2.0 license and
upstream attribution when source code or binaries are redistributed.

### What does the license allow?

In general, Apache License 2.0 allows you to:

- use the firmware,
- modify it,
- create your own builds,
- redistribute source code,
- redistribute binaries,
- incorporate the work into another project,
- use it for private or commercial purposes,

provided that the license conditions are followed.

### Main redistribution obligations

When redistributing source code or binaries, retain as applicable:

1. the `LICENSE` file,
2. relevant copyright notices,
3. relevant attribution/notices,
4. a statement identifying files that you have modified,
5. existing upstream license headers in source files.

Do not remove upstream authorship or copyright notices from files that
already contain them.

### About `.packed.bin` releases

Binary firmware may be distributed through the repository or GitHub
Releases. It is strongly recommended that the repository also include:

```text
LICENSE
NOTICE
README.md
```

If source code is later published, preserve the upstream Apache-2.0
copyright/license headers in inherited files.

### NUSA modifications

For changes made specifically as part of NUSA UV-K5 APRS DIGI, a project
notice may state:

```text
Modified work / NUSA UV-K5 APRS DIGI modifications
Copyright 2026 YF9UAG
```

This **does not replace or claim ownership of upstream code**. Upstream
copyright remains with the respective copyright holders.

### No warranty

Custom firmware can cause unexpected radio behavior and may require
recovery/reflashing.

The Apache License 2.0 provides the work without warranties to the extent
permitted by applicable law.

### Recommended GitHub files

```text
LICENSE
NOTICE
LICENSE_GUIDE_ID_EN.md
```

The `LICENSE` file should contain the unmodified Apache License 2.0 text.

---

## Upstream references / Referensi upstream

- `bg7nzl/uv-k5-v1-firmware`
  - https://github.com/bg7nzl/uv-k5-v1-firmware
- `DualTachyon/uv-k5-firmware`
  - https://github.com/DualTachyon/uv-k5-firmware
- `phdlee/uvk5cec`
  - https://github.com/phdlee/uvk5cec

> This document is a practical project-maintenance guide, not legal advice.
