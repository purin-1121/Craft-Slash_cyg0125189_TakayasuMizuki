@echo off
cd /d %~dp0

:: --- 設定エリア ---
:: フォント名（システム上の名前）
set FONT_NAME=Makinas-4-Flat
:: 出力先（Fontsフォルダ直下）
set OUTPUT_PATH=..\myfont.spritefont
:: ------------------

echo フォント作成を開始します...

:: 実行コマンド
MakeSpriteFont.exe "%FONT_NAME%" "%OUTPUT_PATH%" /FontSize:72 /CharacterRegion:0x20-0x7F /CharacterRegion:0x3000-0x30FF /CharacterRegion:0x4E00-0x9FAF /FastPack

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] 作成に失敗しました。
    echo 以下の原因が考えられます：
    echo 1. フォント名「%FONT_NAME%」がシステムにインストールされていない
    echo 2. フォントサイズ(480)に対してテクスチャサイズが大きすぎる（メモリ不足）
    echo 3. 漢字の範囲が広い
) else (
    echo.
    echo [SUCCESS] 完了しました！
    echo 保存先: %OUTPUT_PATH%
)

pause