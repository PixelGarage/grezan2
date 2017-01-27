/**
 * This file contains all Drupal behaviours of the Apia theme.
 *
 * Created by ralph on 05.01.14.
 */

(function ($) {

  /**
   * This behavior adds shadow to header on scroll.
   *
   */
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
   * Allows full size clickable items.
   Drupal.behaviors.fullSizeClickableItems = {
    attach: function () {
      var $clickableItems = $('.node-link-item.node-teaser .field-group-div')
        .add('.node-team-member.node-teaser .field-group-div');

      $clickableItems.once('click', function () {
        $(this).on('click', function () {
          window.location = $(this).find("a:first").attr("href");
          return false;
        });
      });
    }
  };
   */

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
