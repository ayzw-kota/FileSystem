# ファイルシステムエミュレータ

このプロジェクトは、ファイルシステムエミュレータをC言語で実装したものです。ファイルシステムは、仮想ブロックデバイス上で動作し、inodeやファイルテーブルを使用してファイルやディレクトリを管理します。

## 機能

このファイルシステムエミュレータには、以下の基本的な機能が含まれています：

- **ディレクトリ一覧表示 (ls)**: 現在のディレクトリ内のファイルやディレクトリを一覧表示します。
- **ディレクトリ移動 (cd)**: ディレクトリを移動します。

## ファイル概要

- `main.c`: メインプログラム。ファイルシステムの初期化、テスト、ユーザーインターフェースを提供します。
- `blockdevice.c`: 仮想ブロックデバイスの操作を行うモジュール。ブロックデバイスの読み書きを実装します。
- `blockdevice.h`: `blockdevice.c`のヘッダファイル。ブロックデバイスに関連する関数のプロトタイプを定義します。
- `inode.c`: inodeの管理を行うモジュール。ファイルやディレクトリに関連するメタデータの管理を行います。
- `inode.h`: `inode.c`のヘッダファイル。inodeに関連する関数のプロトタイプを定義します。
- `table.c`: ファイルテーブルの管理を行うモジュール。ファイルのオープン、クローズ、読み書きの管理を行います。
- `table.h`: `table.c`のヘッダファイル。ファイルテーブルに関連する関数のプロトタイプを定義します。

## コンパイル方法

このプロジェクトは、標準的なCコンパイラ（例えば`gcc`）を使用してコンパイルできます。以下はコンパイルコマンドの例です。

```bash
gcc -o filesystem main.c blockdevice.c inode.c table.c
```

## 使用方法

コンパイル後、生成された実行ファイルとファイルシステムのルートディレクトリを表すバイナリファイルを同じディレクトリに配置し、以下のように実行します。
```
./filesystem
以下のコマンドを使用してファイルシステムを操作します：
```

- **ls**: 現在のディレクトリ内のファイルとディレクトリを一覧表示します。
- **cd <dirname>**:  指定したディレクトリに移動します。


## 注意事項
-このプロジェクトは学習目的で作成されたものであり、実際のファイルシステムとして使用することは推奨されません。

## ライセンス

このプロジェクトはMITライセンスの下でライセンスされています。
