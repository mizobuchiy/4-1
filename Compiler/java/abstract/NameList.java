abstract class Name {
  private String _firstName;
  private String _lastName;

  Name(String firstName, String lastName) {
    _firstName = firstName;
    _lastName = lastName;
  }

  abstract String title();

  String nameWithTitle() {
    return (title() + ' ' + _firstName);
  }
}

class NameForMan extends Name {
  NameForMan(String firstName, String lastName) {
    super(firstName, lastName);
  }

  String title() {
    return ("Mr.");
  }
}

class NameForWoman extends Name {
  NameForWoman(String firstName, String lastName) {
    super(firstName, lastName);
  }

  String title() {
    return ("Ms.");
  }
}

class NameList {
  java.util.Vector<Name> _list;

  NameList(String[] list) {
    _list = new java.util.Vector<Name>();

    for (int i = 0; i < list.length; i += 2) {
      String firstName = list[i];
      String lastName = list[i + 1];
      _list.add(firstName.endsWith("ko") ? new NameForWoman(firstName, lastName)
                                         : new NameForMan(firstName, lastName));
    }
  }

  void print() {
    for (int i = 0; i < _list.size(); ++i) {
      System.out.println(_list.elementAt(i).nameWithTitle());
    }
  }

  public static void main(String[] args) {
    (new NameList(args)).print();
  }
}
