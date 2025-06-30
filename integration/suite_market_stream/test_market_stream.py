import pytest
from ..common import MarketStream
import tempfile
import subprocess
import shutil
import time
import os
import platform

@pytest.mark.parametrize("symbol", [
    "BTCUSDT",
    "TRXBTC",
])
class TestMarketStream:
    @pytest.fixture(scope='class', autouse=True)
    def setup_teardown_class(self, request, tmp_path_factory, terry_path):
        cls = request.cls
        cls.temp_dir = tmp_path_factory.mktemp(cls.__name__ + "_tmp")
        cls.timer = 60  # seconds
        
        cls.terry_path = terry_path
        assert os.path.isfile(cls.terry_path), "terry bin does not exist"

        yield

        shutil.rmtree(str(self.temp_dir))

    @pytest.fixture(scope='function', autouse=True)
    def setup_teardown_method(self, symbol):
        self.symbol = symbol
        yield

    def test_save_load_cycle(self):
        # Set SPDLOG_LEVEL=off
        os.environ["SPDLOG_LEVEL"] = "off"
        
        market_stream = MarketStream(self.symbol)

        # Run streamsave
        stream_save_stdout = self.run_streamsave_command(
            symbol=self.symbol, save_path=str(self.temp_dir), timer=self.timer)

        # Run streamload
        stream_load_stdout = self.run_streamload_command(saved_stream_path=str(self.temp_dir))

        assert stream_load_stdout == stream_save_stdout, "Loaded stream not equal to saved"

        # To be sure that test's recorder stream fully cover terry's one
        time.sleep(2)
        
        market_stream.compare_with_terry_stream(stream_load_stdout)

    def run_streamsave_command(self, symbol, save_path, timer):

        # Specify the command to run
        command = [
            str(self.terry_path),
            "stream",
            "save",
            "--symbol", symbol,
            "--output-dir", save_path,
            "--timer", str(timer),
            "--print-stream"
        ]

        # Run the command using subprocess
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        assert result.returncode == 0, f"Error: {result.stderr}"
        
        return result.stdout

    def run_streamload_command(self, saved_stream_path):

        # Specify the command to run
        command = [
            str(self.terry_path),
            "stream",
            "load",
            "--stream-dir", saved_stream_path
        ]

        # Run the command using subprocess
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        
        assert result.returncode == 0, f"Error: {result.stderr}"
        
        return result.stdout
    