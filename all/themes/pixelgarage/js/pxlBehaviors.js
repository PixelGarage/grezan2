/**
 * This file contains all Drupal behaviours of the Apia theme.
 *
 * Created by ralph on 05.01.14.
 */

(function ($) {

  /**
   * This behavior adds shadow to header on scroll.
   *
  Drupal.behaviors.addHeaderShadow = {
    attach: function (context) {
      $(window).on("scroll", function () {
        if ($(window).scrollTop() > 10) {
          $("header.navbar .container").css("box-shadow", "0 4px 3px -4px gray");
        }
        else {
          $("header.navbar .container").css("box-shadow", "none");
        }
      });
    }
  };
   */

  /**
   * Anchor menus: Scrolls smoothly to anchor due to menu click.
   */
  Drupal.behaviors.smoothScrolltoAnchors = {
    attach: function(context, settings) {
      $(function() {
        $('.menu li.leaf a').click(function() {
          var anchorPos = this.href.indexOf('#');
          // no anchor available, perform click
          if (anchorPos == -1) return true;

          // menu item references anchor, get anchor target
          var target = $(this.href.slice(pos));
          if (target.length) {
            $('html, body').stop().animate({
              scrollTop: target.offset().top
            }, 1000, 'swing');
            return false;
          }
          // no target available, perform click
          return true;
        });
      });
    }
  };

  /**
   * Make equal column heights on front page.
   */
  Drupal.behaviors.equalColumnHeights = {
    attach: function (context) {
      var maxHeight = 0,
        calcMaxHeight = function () {
          // context is column
          $(this).height('auto');
          var height = $(this).height();
          if (height > maxHeight) {
            maxHeight = height;
          }
        },
        makeEqualColumns = function () {
          // equal heights in call2action teasers
          maxHeight = 0;
          var call2ActionColumns = $('.front .node-call2action.node-teaser > .row > .col-sm-12');
          call2ActionColumns.each(calcMaxHeight);
          call2ActionColumns.height(maxHeight);

          // equal heights in apartment teasers
          maxHeight = 0;
          var apartmentColumns = $('.node-apartment.node-teaser > .row > .col-sm-12');
          apartmentColumns.each(calcMaxHeight);
          apartmentColumns.height(maxHeight);

          // equal heights in private function teasers
          maxHeight = 0;
          var privateFunctionColumns = $('.node-locality.node-teaser > .row > .col-sm-12');
          privateFunctionColumns.each(calcMaxHeight);
          privateFunctionColumns.height(maxHeight);

        },
        resetColumns = function () {
          var call2ActionColumns = $('.node-call2action.node-teaser > .row > .col-sm-12');
          call2ActionColumns.height('auto');

          var apartmentColumns = $('.node-apartment.node-teaser > .row > .col-sm-12');
          apartmentColumns.height('auto');

          var privateFunctionColumns = $('.node-locality.node-teaser > .row > .col-sm-12');
          privateFunctionColumns.height('auto');
        };

      $(window).on('load resize', function(ev) {
        //
        // only for width > 480 (more than one columns)
        if ($(window).width() >= 480) {
          makeEqualColumns();
        }
        else {
          resetColumns();
        }
      });
    }
  };

  /**
   * Allows full size clickable items.
   */
   Drupal.behaviors.fullSizeClickableItems = {
    attach: function () {
      var $clickableItems = $('.node-call2action.node-teaser');

      $clickableItems.once('click', function () {
        $(this).on('click', function (ev) {
          //
          // exlude apartements list from call2action link, follow apartment links instead)
          var $exclude = $(this).find('.view-apartements');
          if ($exclude.length > 0) {
            var offset = $exclude.offset(),
              width = $exclude.width(),
              height = $exclude.height(),
              x = ev.pageX,
              y = ev.pageY;

            if (x >= offset.left && x < (offset.left + width) && y >= offset.top && y < (offset.top + height)) {
              return true;
            }
          }

          //
          // follow call2action link for click on whole call2action node (except excluded area)
          window.location = $(this).find('.field-name-field-link .field-items a:first').attr("href");
          return false;
        });
      });
    }
  };

  /**
   * Swaps images from black/white to colored on mouse hover.
   Drupal.behaviors.hoverImageSwap = {
    attach: function () {
      $('.node-project.node-teaser .field-name-field-images a img').hover(
        function () {
          // mouse enter
          src = $(this).attr('src');
          $(this).attr('src', src.replace('teaser_bw', 'teaser_normal'));
        },
        function () {
          // mouse leave
          src = $(this).attr('src');
          $(this).attr('src', src.replace('teaser_normal', 'teaser_bw'));
        }
      );
    }
  }
   */

  /**
   * Open file links in its own tab. The file field doesn't implement this behaviour right away.
   Drupal.behaviors.openDocumentsInTab = {
    attach: function () {
      $(".field-name-field-documents").find(".field-item a").attr('target', '_blank');
    }
  }
   */

})(jQuery);
