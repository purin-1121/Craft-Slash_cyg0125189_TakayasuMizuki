from pathlib import Path
import re

# ============================================================
# フォルダ設定
# ============================================================

# このPythonファイルがあるToolsフォルダの親をプロジェクトのルートにする
ROOT = Path(__file__).resolve().parent.parent

# データ構造体のヘッダファイルが入っているフォルダ
DATA_DIR = ROOT / "Game" / "GamePlay" / "Data"

# JSONファイルが入っているフォルダ
JSON_DIR = ROOT / "Resources" / "Jsons"

# 自動生成したヘッダファイルの出力先
OUTPUT_DIR = ROOT / "Common" / "Manager" / "Resources"

# データ型とJSONファイルの対応表を生成するファイル
DATA_PATH_OUTPUT = OUTPUT_DIR / "DataPath.h"

# ゲームオブジェクト型とデータ型の対応表を生成するファイル
OBJECT_TYPE_OUTPUT = OUTPUT_DIR / "ObjectDataType.h"

JSON_NAME_OVERRIDES = {}

# データ型名と対応するゲームオブジェクト名の例外設定
OBJECT_NAME_OVERRIDES = {}

# オブジェクトのヘッダファイルを検索するときに除外するフォルダ
EXCLUDED_DIRS = {
    ".git",
    ".vs",
    "x64",
    "Debug",
    "Release",
    "ipch",
    "Tools",
    "packages",
}


# ============================================================
# 共通処理
# ============================================================

def get_include_path(path):
    """
    ヘッダファイルのパスをプロジェクトルートからの相対パスに変換する。
    C++の#includeで使用できるように区切り文字を/に統一する。
    """
    return path.relative_to(ROOT).as_posix()


def find_object_header(object_name):
    """
    指定されたゲームオブジェクトのヘッダファイルを探す。

    ファイル名が「オブジェクト名.h」であるものを検索し、
    ファイル内に指定されたクラスまたは構造体があるか確認する。
    """
    pattern = re.compile(
        r"\b(class|struct)\s+" + re.escape(object_name) + r"\b"
    )

    # プロジェクト内から、指定した名前のヘッダファイルを検索する
    for header in ROOT.rglob(f"{object_name}.h"):

        # 検索対象から除外するフォルダにあるファイルは無視する
        if any(part in EXCLUDED_DIRS for part in header.parts):
            continue

        try:
            # ヘッダファイルを読み込む
            # 読み込めない文字があっても検索を続けられるようにする
            source = header.read_text(
                encoding="utf-8",
                errors="ignore"
            )
        except OSError:
            # ファイルを開けなかった場合は次の候補を検索する
            continue

        # ファイル内に指定したクラスまたは構造体があれば、そのファイルを返す
        if pattern.search(source):
            return header

    # 対応するヘッダファイルが見つからなかった場合
    return None


def get_object_name(data_name):
    """
    データ型名から対応するゲームオブジェクト型名を求める。

    通常はデータ型名の末尾にある「Data」を取り除く。
    例：EnemyData → Enemy

    命名規則が異なる型は、例外設定を優先する。
    """
    if data_name in OBJECT_NAME_OVERRIDES:
        return OBJECT_NAME_OVERRIDES[data_name]

    if data_name.endswith("Data"):
        return data_name[:-4]

    # データ型名からオブジェクト型名を推測できない場合
    return None


def write_header(path, lines):
    """
    生成したC++コードをヘッダファイルに書き込む。

    出力先フォルダが存在しない場合は自動的に作成する。
    文字コードはUTF-8に統一する。
    """
    path.parent.mkdir(parents=True, exist_ok=True)

    # 各行を改行で連結し、末尾にも改行を付ける
    content = "\n".join(lines).rstrip() + "\n"

    # ヘッダファイルとして保存する
    path.write_text(content, encoding="utf-8")

    print(f"生成完了: {path}")


# ============================================================
# JSONファイルのパス対応表を生成
# ============================================================

def generate_data_paths(data_headers):
    """
    各データ型とJSONファイルの対応表を生成する。

    生成例：
    template<>
    struct JsonDataPath<EnemyData>
    {
        static constexpr const char* value =
            "Resources/Jsons/EnemyData.json";
    };
    """
    lines = [
        "#pragma once",
        "",
        "// Pythonによる自動生成ファイル。直接編集しないこと。",
        "",
    ]

    # 生成するヘッダファイルに必要な#includeを格納する
    includes = []

    # データ型とJSONファイルの対応定義を格納する
    entries = []

    # 使用する型をあらかじめテンプレート宣言しておく
    templateEntries = []

    for header in data_headers:

        # ヘッダファイル名から拡張子を除いたデータ型名を取得する
        data_name = header.stem
        # データ型名からJSONファイル名を決定する
        json_name = JSON_NAME_OVERRIDES.get(
            data_name,
            f"{data_name}.json"
        )

        json_path = JSON_DIR / json_name

        # 対応するJSONファイルが存在しない場合は警告して次へ進む
        if not json_path.is_file():
            print(f"[警告] JSONファイルがありません: {json_path}")
            continue

        # データ型を定義しているヘッダをインクルード対象に追加する
        includes.append(
            f'#include "{get_include_path(header)}"'
        )

        # JSONファイルのパスをプロジェクトルートからの相対パスに変換する
        relative_json_path = json_path.relative_to(ROOT).as_posix()

        # C++のテンプレート特殊化を生成する
        entries.extend([
            "template<>",
            f"struct JsonDataPath<{data_name}>",
            "{",
            f'    static constexpr const char* value = "{relative_json_path}";',
            "};",
            "",
        ])

        templateEntries.extend([
            f"template struct JsonDataPath<{data_name}>;"
        ])

    # 重複した#includeを取り除き、順番を統一する
    lines.extend(sorted(set(includes)))

    # JsonDataPathの基本テンプレートを宣言する
    lines.extend([
        "",
        "template<class T>",
        "struct JsonDataPath;",
        "",
    ])

    # 各データ型に対応するJSONパスの定義を追加する
    lines.extend(entries)

    # テンプレート宣言を追加する
    lines.extend(templateEntries)

    # 生成したコードをヘッダファイルに保存する
    write_header(DATA_PATH_OUTPUT, lines)


# ============================================================
# ゲームオブジェクト型とデータ型の対応表を生成
# ============================================================

def generate_object_data_types(data_headers):
    """
    ゲームオブジェクト型とデータ型の対応表を生成する。

    生成例：
    template<>
    struct ObjectDataType<Enemy>
    {
        using Type = EnemyData;
    };

    これにより、Create<Enemy>()からEnemyDataを取得できるようにする。
    """
    lines = [
        "#pragma once",
        "",
        "// Pythonによる自動生成ファイル。直接編集しないこと。",
        "",
    ]

    # ゲームオブジェクトとデータ型の定義に必要なヘッダを格納する
    includes = []

    # ObjectDataTypeの特殊化を格納する
    entries = []

    for data_header in data_headers:

        # ヘッダファイル名からデータ型名を取得する
        data_name = data_header.stem

        # データ型名から対応するゲームオブジェクト型名を求める
        object_name = get_object_name(data_name)

        # オブジェクト型名を推測できない場合は警告して次へ進む
        if not object_name:
            print(
                f"[警告] オブジェクト型名を判定できません: {data_name}"
            )
            continue

        # 対応するゲームオブジェクトのヘッダを検索する
        object_header = find_object_header(object_name)

        # ヘッダが見つからない場合は警告して次へ進む
        if object_header is None:
            print(
                f"[警告] オブジェクトのヘッダが見つかりません: "
                f"{object_name}.h（データ型: {data_name}）"
            )
            continue

        # データ型とゲームオブジェクト型のヘッダをインクルード対象に追加する
        includes.append(
            f'class {object_name};'
        )
        includes.append(
            f'class {object_name};'
        )

        # ゲームオブジェクト型とデータ型の対応定義を生成する
        entries.extend([
            "template<>",
            f"struct ObjectDataType<{object_name}>",
            "{",
            f"    using Type = {data_name};",
            "};",
            "",
        ])

    # 重複した#includeを取り除き、順番を統一する
    lines.extend(sorted(set(includes)))

    # ObjectDataTypeの基本テンプレートを宣言する
    lines.extend([
        "",
        "template<class T>",
        "struct ObjectDataType;",
        "",
    ])

    # 各オブジェクト型に対応するデータ型の定義を追加する
    lines.extend(entries)

    # 生成したコードをヘッダファイルに保存する
    write_header(OBJECT_TYPE_OUTPUT, lines)


# ============================================================
# メイン処理
# ============================================================

def main():
    print(f"プロジェクトルート: {ROOT}")
    print(f"データ検索フォルダ: {DATA_DIR}")

    if not DATA_DIR.is_dir():
        print(f"[エラー] データフォルダが見つかりません: {DATA_DIR}")
        return

    data_headers = sorted(DATA_DIR.rglob("*Data.h"))

    print(f"データヘッダ検出数: {len(data_headers)}")

    for header in data_headers:
        print(f"検出: {header}")

    generate_data_paths(data_headers)
    generate_object_data_types(data_headers)

    print("すべての生成処理が完了しました。")


# このPythonファイルを直接実行した場合に、メイン処理を開始する
if __name__ == "__main__":
    main()