var Anyterm = function() {
  this.init();
}

Anyterm.prototype = {
  init: function() {
    $.ajaxSetup({
      ifModified: true,
      cache: false
    });
  },

  terminalContents: '',
  initialTimeout: 50,
  timeoutFactor: 2,
  timeoutId: 0,
  timeout: 50,

  resetTimeout: function() {
    Anyterm.timeout = Anyterm.initialTimeout;
  },
  increaseTimeout: function() {
    Anyterm.timeout *= Anyterm.timeoutFactor;
  },

  getRowCount: function() {
    return Math.round($('#terminal').innerHeight() / $('#metric').innerHeight());
  },
  getColumnCount: function() {
    return Math.round($('#terminal').innerWidth() / $('#metric').innerWidth());
  },

  onOpened: function(data) {
    Anyterm.read();
  },

  open: function(username) {
    $.post('anytermd', { action: 'open', row_count: Anyterm.getRowCount(), column_count: Anyterm.getColumnCount(), username: username, dummy: '' }, Anyterm.onOpened);
  },

  resize: function() {
    $.post('anytermd', { action: 'resize', row_count: Anyterm.getRowCount(), column_count: Anyterm.getColumnCount(), dummy: '' });
  },

  onRead: function(text, status, request) {
    if(status == "notmodified") {
      $('#terminal').html(Anyterm.terminalContents);
      Anyterm.increaseTimeout();
    } else if(status == "success") {
      Anyterm.terminalContents = text;
      Anyterm.resetTimeout();
    } else if(status == "error") {
      $('#terminal').html(text);
      Anyterm.increaseTimeout();
    } 

    Anyterm.timeoutId = window.setTimeout(Anyterm.read, Anyterm.timeout);
  },

  read: function() {
    $('#terminal').load('anytermd?action=read', Anyterm.onRead);
  },

  onWrite: function(data) {
    if(Anyterm.timeoutId > 0) {
      window.clearTimeout(Anyterm.timeoutId);
      Anyterm.timeoutId = 0;
    }

    Anyterm.timeoutId = window.setTimeout(Anyterm.read, Anyterm.initialTimeout);
  },

  write: function(value) {
    $.post('anytermd', { action: 'write', data: value }, Anyterm.onWrite);
  }
};

$(document).ready(function() {
  Anyterm = new Anyterm();
});

$(window).resize(function() {
  Anyterm.resize();
});

$(document).keypress(function(event) {
  Anyterm.write(String.fromCharCode(event.keyCode));
});

var codes = {
  NUL: String.fromCharCode(0x00),
  SOH: String.fromCharCode(0x01),
  STX: String.fromCharCode(0x02),
  ETX: String.fromCharCode(0x03),
  EOT: String.fromCharCode(0x04),
  ENQ: String.fromCharCode(0x05),
  ACK: String.fromCharCode(0x06),
  BEL: String.fromCharCode(0x07),
  BS:  String.fromCharCode(0x08),
  HT:  String.fromCharCode(0x09),
  LF:  String.fromCharCode(0x0A),
  VT:  String.fromCharCode(0x0B),
  FF:  String.fromCharCode(0x0C),
  CR:  String.fromCharCode(0x0D),
  SO:  String.fromCharCode(0x0E),
  SI:  String.fromCharCode(0x0F),
  DLE: String.fromCharCode(0x10),
  DC1: String.fromCharCode(0x11),
  DC2: String.fromCharCode(0x12),
  DC3: String.fromCharCode(0x13),
  DC4: String.fromCharCode(0x14),
  NAK: String.fromCharCode(0x15),
  SYN: String.fromCharCode(0x16),
  ETB: String.fromCharCode(0x17),
  CAN: String.fromCharCode(0x18),
  EM:  String.fromCharCode(0x19),
  SUB: String.fromCharCode(0x1A),
  ESC: String.fromCharCode(0x1B),
  FS:  String.fromCharCode(0x1C),
  GS:  String.fromCharCode(0x1D),
  RS:  String.fromCharCode(0x1E),
  US:  String.fromCharCode(0x1F),
  DEL: String.fromCharCode(0x7F),
  CSI: String.fromCharCode(0x9B),
};

$(document).keydown(function(event) {
  if(!event.ctrlKey && !event.shiftKey && !event.altKey) {
    modifier = '';
  } else if(!event.ctrlKey && event.shiftKey && !event.altKey) {
    modifier = ';2';
  } else if(!event.ctrlKey && !event.shiftKey && event.altKey) {
    modifier = ';3';
  } else if(!event.ctrlKey && event.shiftKey && event.altKey) {
    modifier = ';4';
  } else if(event.ctrlKey && !event.shiftKey && !event.altKey) {
    modifier = ';5';
  } else if(event.ctrlKey && event.shiftKey && !event.altKey) {
    modifier = ';6';
  } else if(event.ctrlKey && !event.shiftKey && event.altKey) {
    modifier = ';7';
  }

  switch(event.which) {
    case 46:
      Anyterm.write(codes.ESC + '[3' + modifier + '~');
      return false;
    case 33:
      Anyterm.write(codes.ESC + '[5' + modifier + '~');
      return false;
    case 34:
      Anyterm.write(codes.ESC + '[6' + modifier + '~');
      return false;
    case 37:
      Anyterm.write(codes.ESC + '[' + (modifier != '' ? '1' + modifier : '') + 'D');
      return false;
    case 38:
      Anyterm.write(codes.ESC + '[' + (modifier != '' ? '1' + modifier : '') + 'A');
      return false;
    case 39:
      Anyterm.write(codes.ESC + '[' + (modifier != '' ? '1' + modifier : '') + 'C');
      return false;
    case 40:
      Anyterm.write(codes.ESC + '[' + (modifier != '' ? '1' + modifier : '') + 'B');
      return false;
  }

  if(event.ctrlKey) {
    switch(event.which) {
      case 50:
      case '@':
        Anyterm.write(codes.NUL);
        return false;
      case 65:
      case 'A':
        Anyterm.write(codes.SOH);
        return false;
      case 66:
      case 'B':
        Anyterm.write(codes.STX);
        return false;
      case 67:
      case 'C':
        Anyterm.write(codes.ETX);
        return false;
      case 68:
      case 'D':
        Anyterm.write(codes.EOT);
        return false;
      case 69:
      case 'E':
        Anyterm.write(codes.ENQ);
        return false;
      case 70:
      case 'F':
        Anyterm.write(codes.ACK);
        return false;
      case 71:
      case 'G':
        Anyterm.write(codes.BEL);
        return false;
      case 72:
      case 'H':
        Anyterm.write(codes.BS);
        return false;
      case 73:
      case 'I':
        Anyterm.write(codes.HT);
        return false;
      case 74:
      case 'J':
        Anyterm.write(codes.LF);
        return false;
      case 75:
      case 'K':
        Anyterm.write(codes.VT);
        return false;
      case 76:
      case 'L':
        Anyterm.write(codes.FF);
        return false;
      case 77:
      case 'M':
        Anyterm.write(codes.CR);
        return false;
      case 78:
      case 'N':
        Anyterm.write(codes.SO);
        return false;
      case 79:
      case 'O':
        Anyterm.write(codes.SI);
        return false;
      case 80:
      case 'P':
        Anyterm.write(codes.DLE);
        return false;
      case 81:
      case 'Q':
        Anyterm.write(codes.DC1);
        return false;
      case 82:
      case 'R':
        Anyterm.write(codes.DC2);
        return false;
      case 83:
      case 'S':
        Anyterm.write(codes.DC3);
        return false;
      case 84:
      case 'T':
        Anyterm.write(codes.DC4);
        return false;
      case 85:
      case 'U':
        Anyterm.write(codes.NAK);
        return false;
      case 86:
      case 'V':
        Anyterm.write(codes.SYN);
        return false;
      case 87:
      case 'W':
        Anyterm.write(codes.ETB);
        return false;
      case 88:
      case 'X':
        Anyterm.write(codes.CAN);
        return false;
      case 89:
      case 'Y':
        Anyterm.write(codes.EM);
        return false;
      case 90:
      case 'Z':
        Anyterm.write(codes.SUB);
        return false;
      case 219:
      case '[':
        Anyterm.write(codes.ESC);
        return false;
      case 220:
      case '\\':
        Anyterm.write(codes.FS);
        return false;
      case 221:
      case ']':
        Anyterm.write(codes.GS);
        return false;
      case 222:
      case '^':
        Anyterm.write(codes.RS);
        return false;
      case 189:
      case '_':
        Anyterm.write(codes.US);
        return false;
      case 188:
      case '?':
        Anyterm.write(codes.DEL);
        return false;
    }
  } else if(event.shiftKey) {
    switch(event.which) {
      case 9:
        Anyterm.write(codes.ESC + '[Z');
        return false;
    }
  } else {
    switch(event.which) {
      case 8:
        Anyterm.write(codes.BS);
        return false;
      case 9:
        Anyterm.write(codes.HT);
        return false;
      case 35:
        Anyterm.write(codes.ESC + 'OF');
        return false;
      case 36:
        Anyterm.write(codes.ESC + 'OH');
        return false;
    }
  }

  console.log(event.which, event);
  return true;
});

