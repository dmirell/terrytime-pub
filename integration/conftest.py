import pytest

def pytest_addoption(parser):
    parser.addoption("--terry-path", action="store", help="Path to the terry binary")

@pytest.fixture(scope='class')
def terry_path(request):
    return request.config.getoption("--terry-path")
