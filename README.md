# eos_console_test

This program is lobby function's test for Epic Online Services (EOS).

## 実行準備

1. EPIC アカウントを作成します
1. [DevPortal](https://dev.epicgames.com/portal/ja/) にサインインし、
1. 「製品を作成」からテスト用の製品（プロダクト）を作成します
1. クライアント作成時に、ロビーが利用できるようにしておきます
1. プロダクトの「EOS SDK認証情報」で「ヘッダーファイルで認証情報を使用する」から「製品の認証情報コード」を作成します。
1. ルートフォルダにcredentials.hを作成し、そこへ貼り付けます
1. 「SDKとリリースノート」から「C SDK」の「EOS-SDK-27379709-v1.16.1」をダウンロードし、ルートフォルダへ解凍します。
1. eos_console_test.slnをvs2022で開きビルドを行います

## 実行内容

- アプリケーション認証情報とログイン情報を使ってEOSに接続し、ロビー作成、属性設定、検索のテストを行う構成になっています

