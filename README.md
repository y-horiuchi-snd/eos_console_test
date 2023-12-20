# eos_console_test

このソースコードは、[EpicOnlineServices](https://dev.epicgames.com/services)(EOS)のロビー機能の動作検証を目的に作られています

---

- [eos\_console\_test](#eos_console_test)
  - [1.注意点](#1注意点)
  - [2.実行準備](#2実行準備)
  - [3.ビルド](#3ビルド)
  - [4.実行内容](#4実行内容)
  - [5.ログイン方式の違い](#5ログイン方式の違い)
  - [6.ログイン、接続の流れ（EOS\_ProductUserIdを取得するまで）](#6ログイン接続の流れeos_productuseridを取得するまで)
  - [7.権利](#7権利)

---

## 1.注意点

   ※本ドキュメントで利用しているWebサイトのスクリーンショットは、2023年12月初頭のものを利用しています。

   利用しているEpic Online Services用のSDKは「EOS-SDK-27379709-v1.16.1」を利用したコードとなっております、

   異なったバージョンを利用する際は、必要な個所を適時修正、読み替えるようにしてください。

---

## 2.実行準備

   [DevPortal](https://dev.epicgames.com/portal/)で、アプリケーション登録を行い、テスト用の認証情報を用意します

1. [DevPortal](https://dev.epicgames.com/portal/)にサインインするにはアカウントが必要です、未作成であれば「Epicアカウント」を作成します

   ![signin](画像/000.png "サインイン")

1. [DevPortal](https://dev.epicgames.com/portal/) にサインインし、「製品を作成」を選びます

   ![initial page](画像/001.png "DevPortal 初期画面")

1. 「製品を作成」からeos_console_test用の製品（プロダクト）を作成します、

   任意の「製品名」を付け、「作成」を行います

   ![create product](画像/002.png "製品を作成")

1. 「製品」から作成したプロダクトを選択します

   ![product](画像/003.png "製品ルート画面")

1. 「製品設定」を選びます、「製品設定」の画面へ遷移するので「クライアント」をクリックします

   ![product setting](画像/004.png "製品設定")

1. 契約同意していない場合、ここで確認が出ます

   ![product client contract](画像/005.png "製品クライアント - 契約要求")

1. 同意直後は下記のような画面が出ることがあります、リロードすることで先に進めます

   ![product client contract reload](画像/006.png "製品クライアント - 契約要求 リロード必要")

1. 「新規クライアントを追加」を選択します

   ![product client](画像/007.png "製品設定 - クライアント")

1. 新しく追加するクライアントの設定を行います、「クライアント名」を設定したら、「新規クライアントポリシーを追加」をクリックします

   ![product create client](画像/008.png "製品設定 - クライアント - 新規クライアント作成")

1. クライアントポリシーは、EOSのどの機能を利用するかの許可設定を行います、

   Peer2Peerを選ぶと今回必要としているlobbies,matchingが利用できるポリシーとなります、

   設定ができたら「新規クライアントポリシーを追加」をクリックします

   <details>
   本来は、アプリケーションに必要な機能のみに絞って有効にするべき項目です
   </details>

   ![product create client policy](画像/009.png "製品設定 - クライアント - クライアントポリシー作成")

1. 「新規クライアントを追加」をクリックしてクライアントを作成します

   ![product client policy setting](画像/010.png "製品設定 - クライアント - クライアントにポリシーを設定")

1. クライアントの設定が完了したら、クライアントの画面にはこのような情報が表示されます

   クライアントが作成できたので、アプリケーションを作成します。

   「Epicアカウントサービス」をクリックします、

   ![product client setting complete](画像/011.png "製品設定 - クライアント - 設定完了")

1. 同意がされていない場合、図のような画面になります（同意する必要があります）。

   ![account service initial](画像/012.png "DevPortal - アカウントサービス初期画面")

1. アプリケーションを作成、アプリケーションにクラアントを関連付けする画面です、

     作成済みのアプリケーションは「アプリケーション」の項目にならんでいます。

     初期状態で一つ用意されているのでそれを利用するか、「アプリケーションを作成」から新規に作成します。

   ![account service](画像/013.png "DevPortal - アカウントサービス画面")

1. 「アプリケーション」からクライアントを設定するアプリケーションの「アクセス許可」を選択します。

   「アクセス許可」ではログイン時に指定する「EOS_EAuthScopeFlags」と同じ設定になるように許可を行う必要があります、

   eos_test_consoleは「Basic Profile」のみ要求しているのでデフォルトの設定のまま「変更を保存」を選択します

   ![account service application access permission](画像/014.png "DevPortal - アカウントサービス画面 - アプリケーション設定 - アクセス許可")

1. 次に「リンク済みのクライアント」を設定します、

   「クライアントを選択」から作成したクライアントを選択し、「変更を保存」を選択します

   ![account service application linked application](画像/015.png "DevPortal - アカウントサービス画面 - アプリケーション設定 - リンク済みのアプリケーション")

1. アプリケーションの最低限の設定が出来ました

   ![account service application setting complete](画像/016.png "DevPortal - アカウントサービス画面 - アプリケーション設定 - 設定完了")

1. 設定が終わったので、アプリケーションの認証情報を作成します、「製品設定」へ移動し、

   「ヘッダーファイルで認証情報を使用する」をクリックします

   ※「SDKとリリースノート」からはSDKがダウンロードできます、後ほど利用します。

   ![product root](画像/017.png "製品設定 - ルート")

1. 「デプロイメント」「クライアント」「アプリケーション」を先ほど作成したものを選択し、

   「「理解しました」をクリックします

   ![product root create header](画像/018.png "製品設定 - ヘッダ作成")

1. 認証用のコードが作成されます、右上にコピーボタンがあるのでコピーして利用します。

   ![product root create header copy](画像/019.png "製品設定 - ヘッダ作成 - コピー")

---

## 3.ビルド

1. eos_console_testのルートフォルダにcredentials.hを作成し、「製品の認証情報コード」を貼り付けます

1. 「SDKとリリースノート」から「C SDK」の「EOS-SDK-27379709-v1.16.1」をダウンロードし、eos_console_testのルートフォルダへ解凍します

1. eos_console_test.slnをvs2022で開きビルドを行います

   ※実行には EOS-SDK-27379709-v1.16.1\SDK\Bin\EOSSDK-Win64-Shipping.dll が必要になります、必要に応じてコピー等行ってください。

---

## 4.実行内容

   アプリケーション認証情報とログイン情報を使ってEOSに接続（必要があればユーザー作成）し、５件ロビー作成、属性設定、検索のテストを行う構成になっています

   動作時は以下のようなログが出ます
```text
Initialize
Authorize
ログインタイプを選択
0:EOS_LCT_Developer , 1:EOS_LCT_AccountPortal
0
DevAuthToolに表示されているIPアドレスとポートを入力してください
localhost:8080
DevNameを入力してください
HOSTING
Connect
LobbyCreate
LobbySetAttributes
LobbyCreate
LobbySetAttributes
LobbyCreate
LobbySetAttributes
LobbyCreate
LobbySetAttributes
LobbyCreate
LobbySetAttributes
wait
search 1
index:0[
 DOUBLE_VAL 11.200000
 BOOLEAN true
 NUMBER 0
 TEST 1
 STR abcde
 ID 00024b70c
]
index:1[
 DOUBLE_VAL 11.200000
 BOOLEAN true
 NUMBER 3
 TEST 1
 STR abcde
 ID 00024b70c
]
index:2[
 DOUBLE_VAL 11.200000
 BOOLEAN true
 NUMBER 1
 TEST 1
 STR abcde
 ID 00024b70c
]
index:3[
 DOUBLE_VAL 11.200000
 BOOLEAN true
 NUMBER 2
 TEST 1
 STR abcde
 ID 00024b70c
]
index:4[
 DOUBLE_VAL 11.200000
 BOOLEAN true
 NUMBER 4
 TEST 1
 STR abcde
 ID 00024b70c
]
search 2
index:0[
 DOUBLE_VAL 11.200000
 BOOLEAN true
 NUMBER 3
 TEST 1
 STR abcde
 ID 00024b70c
]
index:1[
 DOUBLE_VAL 11.200000
 BOOLEAN true
 NUMBER 2
 TEST 1
 STR abcde
 ID 00024b70c
]
LobbyLeave
LobbyLeave
LobbyLeave
LobbyLeave
LobbyLeave
Hello World!
```

## 5.ログイン方式の違い

   EOS_LCT_DeveloperとEOS_LCT_AccountPortalの違い

- EOS_LCT_AccountPortal

     EOS_LCT_AccountPortalはWebブラウザ経由での認証となります。

     特に準備をせずとも使えるため、簡易に試験する場合は便利です

- EOS_LCT_Developer

     EOS_LCT_Developerは開発時のみ利用する機能です

     [DevAuthTool](https://dev.epicgames.com/docs/ja/epic-account-services/developer-authentication-tool)

     EOS-SDK-27379709-v1.16.1\SDK\Tools内にある

     EOS_DevAuthToolを常駐起動することで利用できる簡易認証代理サーバーです

     必要に応じて使い分けることで効率的に開発を進めることが出来ます

## 6.ログイン、接続の流れ（EOS_ProductUserIdを取得するまで）

  PC環境でのログインの簡易なフロー図です

  プロダクト内にユーザーが作られていないと、ユーザーを作成する必要があり、

  作成しない限り接続（EOS_Connect_Login）が出来ない、ということが見落としやすい部分です

```mermaid
graph TD
 A[EOS_Initialize]-->B[EOS_Platform_Create]
 B-->C{EOS_Auth_Login}
 C-->|トークン| E{EOS_Connect_Login}
 E-->|OK|H[EOS_ProductUserId]
 E-->|EOS_InvalidUser| G[EOS_Connect_CreateUser]
 G-->H[EOS_ProductUserId]
```

## 7.権利

© 2023, Epic Games, Inc. Epic、Epic Games、Epic Games のロゴ、Fortnite/フォートナイト、Fortnite/フォートナイトのロゴ、Unreal、Unreal Engine、Unreal Engine のロゴ、Unreal Tournament、Unreal Tournament のロゴは、 米国およびその他の国々における Epic Games, Inc. の商標または登録商標であり、無断で複製、転用、転載、使用することはできません。その他のブランドや製品名は、それらを所有する会社の商標です。
