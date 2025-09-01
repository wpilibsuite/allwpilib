from wpilib import Mechanism2d


def test_create_mechanism():
    m = Mechanism2d(100, 100)
    r1 = m.getRoot("r1", 10, 10)
    l1 = r1.appendLigament("l1", 4, 3)
    l2 = l1.appendLigament("l2", 4, 3)
    assert l2 is not None

    # TODO... check that they do something?
