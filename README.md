# moNa2 v2 (DYA Studio 対応版)

このリポジトリは [moNa2 v2](https://github.com/sayu-hub/zmk-config-moNa2) 用 ZMK Config を、[cormoran](https://github.com/cormoran) さんが公開している **[DYA Studio](https://studio.dya.cormoran.works/)** に対応させたものです。

DYA Studio は ZMK Studio をベースに、

- キーマップ／レイヤーの GUI 編集
- **マクロの作成・編集・削除（ファームウェア再ビルド不要）**
- **コンボの追加・編集・削除（ファームウェア再ビルド不要）**
- トラックボール (PMW3610) のスクロール量・軸反転・自動マウスレイヤーなど **Runtime Input Processor 経由での動的設定**
- BLE プロファイル管理 / Settings RPC

を Web から行えるようにした拡張版です。

## DYA Studio の使い方

1. 本リポジトリをビルドし、生成された `mona2_r-...uf2`（中央側＝右手）と `mona2_l-...uf2`（周辺側＝左手）をそれぞれの XIAO BLE に書き込みます。
2. 中央側（右手）を **USB ケーブル** で PC に接続します。
3. Chrome / Edge などの Web Serial 対応ブラウザで **[https://studio.dya.cormoran.works/](https://studio.dya.cormoran.works/)** を開きます。
4. DYA Studio 側で「USB で接続」を押し、シリアルポートとして `mona2` を選択します。このブランチでは Studio ロックを無効にしているため、アンロック操作は不要です。
5. キーマップ／マクロ／コンボ／トラックボール設定を編集できます。

旧 `update`（`7ba1c8c`）の7レイヤーをベースにしています：`BASE`、`FUNCTION`、`NUM`、`F-KEYS`、`BLE`、`SCROLL`、`MOUSE`。親指右端は旧版と同じ短押し `Esc`／長押し右 Ctrl です。

### マクロを DYA Studio から使う

マクロのランタイム編集は [zmk-feature-runtime-macro](https://github.com/cormoran/zmk-feature-runtime-macro) が担当します。

1. DYA Studio のマクロ画面で新しいマクロを作成し、名前と内容を設定して保存します。
2. マクロ一覧に表示される **スロット番号 (0〜7)** を確認します。
3. DYA Studio のキーマップエディタで、任意のキーに `&rmacro <スロット番号>` を割り当てます。

キーマップ側にはすでに `#include <behaviors/runtime_macro.dtsi>` を入れてあるので、`&rmacro` は最初から選択できます。

#### 保存済みのコピペマクロも接続先に合わせる

保存済みマクロのスロット番号・割り当ては変えず、マクロの中の該当するステップだけ変更します。DYA Studio のマクロ編集画面で、`Ctrl+C` などのキー送信を **Host Key (Windows / Apple)** のタップに置き換え、両方のキーを選びます。

| 操作 | Windows / USB / BT 5 | Apple / BT 2〜4 |
| --- | --- | --- |
| コピー | `Ctrl+C` | `⌘C` |
| 貼り付け | `Ctrl+V` | `⌘V` |
| 切り取り | `Ctrl+X` | `⌘X` |
| 全選択 / 取り消し | `Ctrl+A` / `Ctrl+Z` | `⌘A` / `⌘Z` |

`Ctrl 押下 → C タップ → Ctrl 解放` のような3ステップは、この動作の1タップに置き換えます。それ以外の手順や待機時間は維持します。固定の `Ctrl` が入った既存マクロを、ファームウェアが自動で書き換えることはありません。静的定義の `&BT0`、`&BT1`、`&screenshot`、`&henkan` も互換用に残しています。

### コンボを DYA Studio から使う

コンボは [zmk-feature-runtime-combo](https://github.com/cormoran/zmk-feature-runtime-combo) で編集できます。`config/mona2.keymap` の既定値は次の通りです（キー名は BASE 上の表記）。

| スロット | 操作 | 同時押し | 反応するレイヤー |
| --- | --- | --- | --- |
| 0 | 全選択 | A + S | BASE / MOUSE |
| 1 | Tab | W + E | 全レイヤー |
| 2 | コピー | S + D | 全レイヤー |
| 3 | 貼り付け | D + F | 全レイヤー |
| 4 | 切り取り | F + G | 全レイヤー |
| 5 | Shift+Tab | E + R | 全レイヤー |
| 6 | FUNCTION／右 Shift | J + K | 全レイヤー |
| 7 | 取り消し | Z + X | 全レイヤー |
| 8 | やり直し | X + C | 全レイヤー |
| 9 | クリップボード | C + V | 全レイヤー |

全コンボは旧版と同じキー位置とレイヤー条件で、入力猶予は 50 ms（直前のアイドル待ちなし）です。10〜15 は既定で空きです。Windows では `Win+V`、Apple では同じ HID キーが `⌘V` として送信されます。Apple でクリップボード履歴を開く設定ではありません。

**保存済みのコンボが優先されます。** 前版の設定が残っていたら、バックアップ後に必要なスロットだけ既定値へ戻すか修正してください。特にスロット 0（旧 DYA 版の BLE/Esc）と、2〜4 の発火条件を確認します。全初期化や既存マクロ・ペアリングの削除は不要です。

### Bluetooth で Windows / Apple を切り替える

BLE レイヤー（旧版と同じ、中央寄りの `[` キーを長押し）の右上段で選びます。ZMK の `BT_SEL` は0始まりですが、下の表は1始まりです。

| スロット | ホスト | 選択キー（BASE 表記） |
| --- | --- | --- |
| 1 | Windows | Y |
| 2 | Mac | U |
| 3 | iPad | I |
| 4 | iPhone | O |
| 5 | 予備（Windows 側の動作） | P |

- 実際の HID 出力先が BLE の 2〜4 のときだけ Apple 用になります。OSの推測やレイヤーの切替はしません。USB でタイピング中は Windows 側の動作です。BLE の試験は USB を外して行ってください。
- BASE 左下の Ctrl と Windows の位置は、Apple ではそれぞれ `⌘` と `Ctrl` になります。
- 親指の変換キーは Windows で旧版どおり変換、Apple では `かな`（`LANG1`）。長押しは元の左 Alt のままです。旧版の `Shift+Caps` の位置は Apple では `英数`（`LANG2`）を送ります。
- BLE の `/`（選択中のペアリング消去）と右端の親指キー（全消去）は、短押しでは何もせず、**1.2秒の長押し**で実行します。既存のペアリングを移動したり消去する必要はありません。別の番号に登録済みなら、番号を確認してから必要なスロットだけを再ペアリングしてください。

#### 仮想デスクトップ / アプリの切り替え

| 操作 | Windows（BT 1 / USB） | Mac / iPad（BT 2 / 3） |
| --- | --- | --- |
| 戻る（FUNCTION の E） | `Ctrl+Win+←` | `⌘⇧Tab` |
| 進む（FUNCTION の R） | `Ctrl+Win+→` | `⌘Tab` |

旧 SCROLL レイヤーの U/I では `Alt+Shift+Tab` / `Alt+Tab` を Apple で `⌘⇧Tab` / `⌘Tab` に切り替えます。FUNCTION の T は `Win+Tab` / Mission Control（`Ctrl+↑`）です。

DYA Studio の保存済みコンボやマクロが `Key Press` の1ステップで `Ctrl+Win+←/→`（左 Ctrl と左 Win）を送る場合も、同じ操作のまま自動で切り替わります。スロットや同時押しのキー位置は変わりません。修飾キーを個別に押す複数ステップのマクロや右側の修飾キーを使った設定は自動変換の対象外なので、必要に応じて該当部分を **Host Key (Windows / Apple)** の1タップに置き換えてください。

BT 4（iPhone）にも Apple 側のショートカットを送りますが、アプリ切り替えの動作は未検証です。Apple 側は最近使った順の切り替えで、固定順のデスクトップ移動とは動作が異なります。

更新前は DYA Studio で既存のキーマップ・マクロ・コンボをエクスポートまたは控えてください。通常の右手用 UF2 を使い、ペアリングや保存設定を維持したい場合は `settings_reset` を使わないでください。保存済みの個別キーが既定値を上書きしている場合も、該当キーだけを確認・修正してください。

### このリポジトリで対応済みの内容

| 対応項目 | ファイル / 箇所 |
| --- | --- |
| ZMK 本体を cormoran さんの fork (`main+dya`, ZMK v0.4 / Zephyr 4.1 系) に切替 | `config/west.yml` |
| DYA Studio 基盤モジュール (custom-settings / ble-management / settings-rpc / runtime-input-processor / runtime-sensor-rotate) | `config/west.yml` |
| **マクロ・コンボのランタイム編集モジュール (runtime-macro / runtime-combo) を追加** | `config/west.yml` |
| Studio 系 CONFIG (`CONFIG_ZMK_STUDIO`, `CONFIG_ZMK_CUSTOM_SETTINGS`, `CONFIG_ZMK_BLE_MANAGEMENT`, `CONFIG_ZMK_SETTINGS_RPC`, `CONFIG_ZMK_RUNTIME_INPUT_PROCESSOR`, `CONFIG_ZMK_RUNTIME_MACRO`, `CONFIG_ZMK_RUNTIME_COMBO` ほか) | `config/mona2_r.conf` |
| 静的コンボ (`zmk,combos`) を `runtime_combo_defaults` へ移行 | `config/mona2.keymap` |
| `&rmacro` (ランタイムマクロ behavior) を利用可能に | `config/mona2.keymap` |
| **DYA Studio 診断タブ用モジュール (kscan-diagnostics / input-stream / devtool) を追加** | `config/west.yml`, `config/mona2_r.conf`, `config/mona2_l.conf` |
| **トラックボールドライバを cormoran さんの Studio RPC 対応版へ移行** | `config/west.yml`, `mona2_r.overlay`, `Kconfig.defconfig`, `mona2_r.conf` |
| トラックボール処理を Runtime Input Processor に置換 | `boards/shields/mona2/mona2.dtsi`, `mona2_r.overlay` |
| Studio ロックを無効化し、USB 接続時にアンロック不要 | `config/mona2_r.conf` |
| 全レイヤーに `display-name` を設定 | `config/mona2.keymap` |
| `studio-rpc-usb-uart` snippet (中央側) / ボード名を `xiao_ble/nrf52840/zmk` へ | `build.yaml` |

> **Note:**
> - DYA Studio で行った変更は **中央側 (右手) の Flash** に保存されます。初期化したい場合は `settings_reset` ファームウェアを書き込んでください。
> - **ZMK v0.4 系への移行に伴い、以前の設定（保存済みキーマップ・トラックボール設定）は引き継がれません。** 書き込み後に DYA Studio から設定し直してください。

## ZMK v0.4 系への移行で変わった点

`v0.3-branch+dya` (Zephyr 3.5) から `main+dya` (Zephyr 4.1) に上げたことで、以下の破壊的変更に追従しています。

| 変更 | 対応 |
| --- | --- |
| ボード名 `seeeduino_xiao_ble` → `xiao_ble`、ZMK 用バリアント `/nrf52840/zmk` が必須 | `build.yaml` |
| Zephyr 4.1 が純正 `pixart,pmw3610` ドライバを同梱したため、旧 compatible `pixart,pmw3610` が使えなくなった | トラックボールドライバを cormoran さんの版へ移行（下記参照） |
| `zmk-rgbled-widget` の `v0.3` ブランチは Zephyr 3.5 向け | `west.yml` (`v0.3` → `main`) |
| endpoints API が `zmk_endpoints_send_mouse_report()` → `zmk_endpoint_send_mouse_report()` に改名 | `zmk-module-mouse-gesture-rpc` の `main` に取り込み済み |
| 再利用 GitHub Actions ワークフローが `@v0.3.0` のままだと Zephyr 4.1 でビルドできない | `.github/workflows/build.yml` (`@main`) |

## DYA Studio の診断タブ対応

DYA Studio の診断ページには専用モジュールが必要なパネルがあり、
未対応だと「このキーボードでは利用できません」と表示されます。以下を有効にしています。

| DYA Studio のパネル | モジュール | 主な CONFIG |
| --- | --- | --- |
| キースイッチ（キー押下統計・チャタリング検出） | [zmk-feature-kscan-diagnostics](https://github.com/cormoran/zmk-feature-kscan-diagnostics) | `CONFIG_ZMK_KSCAN_DIAGNOSTICS` |
| ↑のライブ表示 | [zmk-feature-input-stream](https://github.com/cormoran/zmk-feature-input-stream) | `CONFIG_ZMK_INPUT_STREAM_FEATURE` |
| トラックボールセンサー（PMW3610 の状態・表面診断） | [zmk-driver-pmw3610-with-custom-studio-rpc](https://github.com/cormoran/zmk-driver-pmw3610-with-custom-studio-rpc) | `CONFIG_ZMK_PMW3610_STUDIO_RPC` |
| スタック使用量 | [zmk-module-devtool](https://github.com/cormoran/zmk-module-devtool) | `CONFIG_ZMK_DEVTOOL_STACK_USAGE` |
| デバイス情報（ビルド・ハードウェア・ランタイム詳細） | [zmk-feature-device-info](https://github.com/cormoran/zmk-feature-device-info) | `CONFIG_ZMK_DEVICE_INFO` |
| 安定性（ウォッチドッグ） | [zmk-feature-watchdog](https://github.com/cormoran/zmk-feature-watchdog) | `CONFIG_ZMK_WATCHDOG` |
| Zephyr settings の閲覧 | [zmk-feature-zephyr-setting-expose](https://github.com/cormoran/zmk-feature-zephyr-setting-expose) | `CONFIG_ZMK_SETTING_EXPOSE` |
| Studio RPC の性能計測 | [zmk-feature-studio-rpc-perf](https://github.com/cormoran/zmk-feature-studio-rpc-perf) | `CONFIG_ZMK_STUDIO_RPC_PERF` |
| OS ごとのデフォルトレイヤー | [zmk-feature-default-layer](https://github.com/cormoran/zmk-feature-default-layer) + [zmk-feature-os-detection](https://github.com/cormoran/zmk-feature-os-detection) | `CONFIG_ZMK_DEFAULT_LAYER`, `CONFIG_ZMK_OS_DETECTION` |

ウォッチドッグはフリーズ・ハードフォルト・予期しないリセットを Flash に記録します。
左右それぞれが自分のインシデントを記録し、Web UI の source セレクタで
「Central」「Peripheral」を切り替えて参照できます。
なお `CONFIG_ZMK_WATCHDOG_FREEZE_DETECT`（既定で有効）は、システムワークキューが
10 秒以上詰まった場合にインシデントを記録して再起動します。
この挙動が不要なら `CONFIG_ZMK_WATCHDOG_FREEZE_DETECT=n` を追加してください。

### OS ごとのデフォルトレイヤー

接続先別ショートカットはレイヤー設定を変更しません。このブランチではレイヤー 0 (`BASE`) を全端末で共通に使います。
レイヤー 1 は `FUNCTION` なので、OS ごとのベースレイヤーに指定しないでください。
BLE の各スロットの割り当ては未設定または 0 で使います。以前の DYA 版で Mac 用にレイヤー 1 を保存していた場合は、DYA Studio の
「OSごとのデフォルトレイヤー」で割り当てを解除するか 0 に戻してください。

> os-detection 自身の `CONFIG_ZMK_OS_DETECTION_LAYER_*` による自動切替は
> default-layer モジュールと競合するため有効にしないでください。

`zmk-feature-default-layer` だけは `main` ではなく `codex/custom-rpc-rewrite` ブランチを
使っています。`main` には DYA Studio 用の custom Studio RPC が入っておらず、
パネルから操作できないためです（DYA2 v2.0 も同じブランチを使用）。

キースイッチ診断は分割キーボードなので、周辺側（左手）の統計を中央側経由で取得するために
`CONFIG_ZMK_SPLIT_RELAY_EVENT` を有効にし、`CONFIG_ZMK_SPLIT_RELAY_EVENT_DATA_LEN=256` を
**左右両方の conf に同じ値で**設定しています。

### トラックボールドライバの移行について

「トラックボールセンサー」パネルは cormoran さんのドライバでしか動かないため、
badjeff さんの `zmk-pmw3610-driver` から
`zmk-driver-pmw3610-with-custom-studio-rpc` へ移行しました。

- devicetree の compatible が `pixart,pmw3610-alt` → **`cormoran,pmw3610`**
- センサー自体の軸反転の既定値は `config/mona2_r.conf` で指定します。
  COROPIT 向けの入力処理は下記を参照してください。
- CPI・軸反転・ダウンシフト時間などは `CONFIG_ZMK_PMW3610_CUSTOM_SETTINGS=y` により
  **DYA Studio から実行時に変更・保存できます**。
- ドライバ実装が変わるため、カーソルの追従感やスリープ復帰の挙動が
  以前と微妙に変わる可能性があります。違和感があれば DYA Studio 側で
  CPI やダウンシフト時間を調整してみてください。

PAW3222 版は従来のドライバ、軸設定、スクロール倍率を維持します
（`config/paw3222.overlay` で上書きしています）。

### 慣性スクロール

マウスジェスチャーの慣性スクロール（スクロールを止めたあと惰性で流れる挙動）は
**既定で無効**です。使いたい場合は DYA Studio の設定画面から有効化してください
（有効にした設定はフラッシュに保存され、次回起動時にも復元されます）。

有効/無効の既定値は `zmk-module-mouse-gesture-rpc` 側でしか変えられません
（入力プロセッサの初期値と設定ストアの初期値の両方にハードコードされているため）。
現在はモジュールの `main` で既定オフになっています。

> 完全に無効でよいなら、`mona2_r.overlay` の `&trackball_central_listener` の
> `input-processors` から `&inertial_scroll` を外す方法もあります。
> ただしその場合は DYA Studio からも有効化できなくなります。

### mouse-gesture-rpc の参照先について

`zmk-module-mouse-gesture-rpc` は、ファームウェア側の開発ブランチ
`expose-mg-set-to-studio` を `main` にマージ済みのため、`main` を参照しています。

ZMK v0.4 で消えた `zmk_endpoints_send_mouse_report()` の改名対応と、
慣性スクロールを既定オフにする変更はモジュール側に取り込まれているので、
以前このリポジトリに置いていた互換シム（`compat/`、`CMakeLists.txt`、
`zephyr/module.yml` の `cmake: .`）は不要になり削除しました。

Web UI の数値入力欄が最初の1桁で固まる問題の修正差分
（`docs/mouse-gesture-web-numberfield.patch`）も `main` へ取り込み済みです。

### PAW3222 版ビルドについて（既存不具合の修正）

Zephyr は devicetree を Kconfig より **先に** 処理するため、`mona2_r.overlay` 内の
`#ifdef CONFIG_TRACKBALL_PAW3222` はクリーンビルドでは常に偽になります。
その結果、これまで `mona2_r-paw3222` という名前で生成されていた成果物は
**実際には PMW3610 版**になっていました。

`config/paw3222.overlay` を追加し、`build.yaml` から `-DEXTRA_DTC_OVERLAY_FILE` で
渡すことで、正しく `pixart,paw3222` が選ばれるよう修正しています。

### ビルド検証状況

このブランチは実機書き込み前に、ローカル (Zephyr SDK 0.17.0 / arm-zephyr-eabi) で
以下 4 構成のビルドが通ることを確認しています。

| 構成 | 結果 | FLASH | RAM |
| --- | --- | --- | --- |
| `mona2_r rgbled_adapter` (PMW3610) | OK | 51.23% | 65.77% |
| `mona2_r rgbled_adapter` (PAW3222) | OK | 50.53% | 65.43% |
| `mona2_l rgbled_adapter` | OK | 27.14% | 25.06% |
| `settings_reset` | OK | 7.31% | 6.66% |

**実機での動作確認（特にトラックボール、マウスジェスチャー、エンコーダ、BLE ペアリング）は未実施です。**

---

## トラックボール (COROPIT / PMW3610)

このブランチには COROPIT 用設定が入っており、ドライバやオーバーレイを手作業で変更する必要はありません。

- センサー側で X / Y を反転し、通常のカーソル移動では入力処理側でも Y を反転します。
- 旧 `update` に合わせて COROPIT は 500 CPI、レイヤー 5 (`SCROLL`) でホイールに切り替わります。スクロール時はカーソル用の Y 補正を打ち消し、PMW3610 の固定倍率は `1/5` です。エンコーダーの 24 ステップ／10 トリガーと BLE の優先接続間隔も旧版の設定です。
- DYA Studio からマウス・スクロール個別の軸や倍率を調整できます。本体に保存した値は、更新後もファームウェアの既定値より優先される場合があります。動きが変わらない場合は、DYA Studio で該当項目の設定を確認してください。
