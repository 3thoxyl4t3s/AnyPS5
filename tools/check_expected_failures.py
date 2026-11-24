import argparse
import sys
import xml.etree.ElementTree as ET


def failing_tests(xml_path: str) -> list[str]:
    try:
        tree = ET.parse(xml_path)
    except FileNotFoundError:
        raise SystemExit(f"{xml_path}: no ctest junit file: ctest did not run or did not write one")
    except ET.ParseError as parse_error:
        raise SystemExit(f"{xml_path}: not valid XML: {parse_error}")
    return [case.get("name") for case in tree.getroot().iter("testcase") if case.find("failure") is not None or case.find("error") is not None]


def main() -> None:
    parser = argparse.ArgumentParser(description="fail when a test failed that the job's expected-failure list does not track")
    parser.add_argument("junit", help="the junit XML ctest wrote with --output-junit")
    parser.add_argument("expected", nargs="+", help="the test names the macOS baseline tracks")
    args = parser.parse_args()
    expected = set(args.expected)
    unexpected = []
    for name in failing_tests(args.junit):
        if name in expected:
            print(f"::warning title=Expected macOS failure::{name} failed, which the macOS baseline tracks")
        else:
            print(f"::error title=Unexpected macOS failure::{name} failed but the macOS baseline does not track it; fix it or add it to the expected list with its issue for the nightly macOS job")
            unexpected.append(name)
    if unexpected:
        raise SystemExit(f"{len(unexpected)} test(s) failed outside the macOS baseline: {', '.join(unexpected)}")


if __name__ == "__main__":
    main()
