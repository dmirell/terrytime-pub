import pytest
import tempfile
import subprocess
import shutil
import json
import asyncio
import os
import platform

@pytest.mark.parametrize("symbol", [
    "BTCUSDT",
    "TRXUSDT",
])
class TestStreamAnalyzer:
    @pytest.fixture(scope='class', autouse=True)
    def setup_teardown_class(self, request, tmp_path_factory, terry_path):
        cls = request.cls
        cls.temp_test_online_dir = tmp_path_factory.mktemp(cls.__name__ + "_test_online_" + "tmp")
        cls.temp_test_local_dir = tmp_path_factory.mktemp(cls.__name__ + "_test_local_" + "tmp")
        cls.temp_test_local_no_ts_jump_dir = tmp_path_factory.mktemp(cls.__name__ + "_test_local_no_ts_jump_" + "tmp")
        cls.duration = 600 # seconds
        cls.strategy_name = "dummy"
        cls.output_json_file_name = "strategy_test_result.json"
        
        cls.terry_path = terry_path
        assert os.path.isfile(cls.terry_path), "terry bin does not exist"

        yield

        shutil.rmtree(str(self.temp_test_online_dir))
        shutil.rmtree(str(self.temp_test_local_dir))
        shutil.rmtree(str(self.temp_test_local_no_ts_jump_dir))

    @pytest.fixture(scope='function', autouse=True)
    def setup_teardown_method(self, symbol):
        self.symbol = symbol
        yield

    def test_stream_analyzer(self):
        # Run test online
        self.run_strategy_test_online_command(output_dir=str(self.temp_test_online_dir))

        # Run local tests
        print("Start run locally.")
        asyncio.run(self.run_local_tests_in_parallel())
        print("Finish run locally.")

        # Read strategy test output jsons
        test_online_json = self.read_strategy_test_result_json(self.temp_test_online_dir)
        test_local_json = self.read_strategy_test_result_json(self.temp_test_local_dir)
        test_local_no_ts_jump_json = self.read_strategy_test_result_json(self.temp_test_local_no_ts_jump_dir)

        # Verify equality
        self.compare_strategy_test_json_results(json_a=test_online_json, json_b=test_local_json)
        self.compare_strategy_test_json_results(json_a=test_online_json, json_b=test_local_no_ts_jump_json)
        self.compare_strategy_test_json_results(json_a=test_local_json, json_b=test_local_no_ts_jump_json)


    def compare_strategy_test_json_results(self, json_a, json_b, min_similarity=0.9):

        print(f"json_a: {json_a}")
        print(f"json_b: {json_b}")
        """
        Compares two JSON objects to ensure they are identical except for
        the fields 'average_order_plan_duration_ms' and 'median_order_plan_duration_ms',
        which are compared using a minimum similarity percentage.

        :param json_a: First JSON object to compare.
        :param json_b: Second JSON object to compare.
        :param min_similarity: Minimum similarity percentage (e.g., 0.9 for 90%).
        :raises AssertionError: If the objects are not identical or similarity is insufficient.
        """
        # Ensure both inputs are dictionaries
        assert isinstance(json_a, dict), "json_a must be a dictionary"
        assert isinstance(json_b, dict), "json_b must be a dictionary"

        # Check if both have the 'error' field
        error_a = json_a.get("error")
        error_b = json_b.get("error")

        if error_a and error_b:
            # If both JSONs have an error, compare the error messages
            assert error_a == error_b, f"Error messages do not match: '{error_a}' != '{error_b}'"
            return  # Exit early if errors match, no need to compare further
        
        # List of fields to compare with percentage similarity
        fields_with_delta = ["average_order_plan_duration_ms", "median_order_plan_duration_ms"]
        
        # Compare all fields except the ones with percentage similarity
        for key in json_a:
            if key in fields_with_delta:
                # Skip these fields for now
                continue
            
            # Ensure the field exists in both JSONs
            assert key in json_b, f"Field '{key}' is missing in json_b"
            
            # Ensure the values are identical
            assert json_a[key] == json_b[key], f"Mismatch for field '{key}': {json_a[key]} != {json_b[key]}"
        
        # Compare fields with percentage similarity
        for key in fields_with_delta:
            value_a = json_a.get(key)
            value_b = json_b.get(key)
            
            # Ensure the field exists in both JSONs
            assert value_a is not None, f"Field '{key}' is missing in json_a: {json_a}"
            assert value_b is not None, f"Field '{key}' is missing in json_b: {json_b}"
            
            # Convert string values to floats (or Decimal if higher precision is needed)
            value_a = float(value_a)
            value_b = float(value_b)
            
            # Calculate percentage similarity
            if value_a == 0 and value_b == 0:
                similarity = 1.0  # Both are zero, consider them 100% similar
            elif value_a == 0 or value_b == 0:
                similarity = 0.0  # One is zero while the other is not
            else:
                similarity = min(value_a, value_b) / max(value_a, value_b)
            
            # Ensure the similarity meets the required threshold
            assert similarity >= min_similarity, (
                f"Mismatch for field '{key}': {value_a} and {value_b} have similarity {similarity:.2%}, "
                f"which is below the required {min_similarity:.2%}"
            )
        
        # Ensure json_b does not have extra fields
        for key in json_b:
            if key not in json_a:
                raise AssertionError(f"Extra field '{key}' found in json_b")

    def read_strategy_test_result_json(self, json_path):
        file_path = os.path.join(json_path, self.output_json_file_name)
        with open(file_path, "r") as file:
            data = json.load(file)
        assert data, "json data is empty"
        return data

    def run_strategy_test_online_command(self, output_dir):
        # Specify the command to run
        command = [
            str(self.terry_path),
            "strategy",
            "test-online",
            "--symbol", self.symbol,
            "--output-dir", output_dir,
            "--duration", str(self.duration),
            "--strategy", self.strategy_name
        ]

        # Run the command using subprocess
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        assert result.returncode == 0, f"Error: {result.stderr}"

    async def run_test_local(self):
        # Run test local
        await asyncio.to_thread(
            self.run_strategy_test_local_command,
            stream_dir=str(self.temp_test_online_dir),
            output_dir=str(self.temp_test_local_dir))

    async def run_test_local_no_ts_jump(self):
        # Run test local with no ts jump
        await asyncio.to_thread(
            self.run_strategy_test_local_command,
            stream_dir=str(self.temp_test_online_dir),
            output_dir=str(self.temp_test_local_no_ts_jump_dir),
            no_ts_jump=True)

    async def run_local_tests_in_parallel(self):
        # Run both tests concurrently
        await asyncio.gather(
            self.run_test_local(),
            self.run_test_local_no_ts_jump()
        )

        print("Both tests finished successfully.")

    def run_strategy_test_local_command(self, stream_dir, output_dir, no_ts_jump=False):
        # Specify the command to run
        command = [
            str(self.terry_path),
            "strategy",
            "test",
            "--stream-dir", stream_dir,
            "--output-json-dir", output_dir,
            "--strategy", self.strategy_name
        ]

        if no_ts_jump:
            command.append("--no-ts-jump")

        # Run the command using subprocess
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        assert result.returncode == 0, f"Error: {result.stderr}"
    